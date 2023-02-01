#!/bin/bash

thisdir=$(readlink -m $(dirname $0))

version=$1


if [ $(id -u) != 0 ]; then
	echo >&2 "Must be root to install this package. Aborting."
	exit 1
fi

if [ -z "$version" ] ; then
	echo >&2 "missing version arg. Aborting installation."
	exit 1
fi

# Ask user to agree on license
bash $thisdir/prompt_linux_license.sh
if [ $? -ne 0 ]
then
	$thisdir/cleanup.sh
	exit 1
fi


rules_path=/etc/udev/rules.d/
set junk ${rules_path}*
shift
first_rule=$1

if [ -f "$first_rule" ] ; then
	# If there are already installed files, check for stamps (ST_PKG_VERSION)
	# Check all versions including the one to be installed and see which is the latest (sort -u|tail -1)
	latest_version=$(
		(
			echo $version
			awk -F '[ \t=]+' '/ST_PKG_VERSION/{print $3}' ${rules_path}*
		) |sort -uV | tail -1
	)

	# latest_version should be our candidate for installation
	if [ $latest_version != "$version" ]; then
		echo >&2 "Not overwriting version $latest_version with an older one ($version)"
		exit 0
	fi
fi

# Finally do the installation
install -D -o root -g root -m 644 -t $rules_path $thisdir/*.rules

echo >&2 "Package installed."
$thisdir/cleanup.sh
