#!/bin/bash

thisdir=$(readlink -m $(dirname $0))

cd $thisdir

# Selective cleanup as self extract may have been
# done in a user-created dir.

# Remove known objects
for item in $(cat pkg_rootdir_content.txt) root
do
        rm -rf $item
done

# Attempt to remove dir only if it's empty
if [ -z "$(ls -A)" ]; then
        rmdir $thisdir
fi
