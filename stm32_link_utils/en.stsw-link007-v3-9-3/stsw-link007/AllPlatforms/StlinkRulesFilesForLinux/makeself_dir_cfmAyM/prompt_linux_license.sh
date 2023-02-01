#!/bin/bash

if [ "$LICENSE_ALREADY_ACCEPTED" ] ; then
	exit 0
fi

display_license() {
cat << EOF
STMicroelectronics Software License Agreement

SLA0048 Rev4/March 2018

Please indicate your acceptance or NON-acceptance by selecting "I ACCEPT" or "I DO NOT ACCEPT" as indicated below in the media.

BY INSTALLING COPYING, DOWNLOADING, ACCESSING OR OTHERWISE USING THIS SOFTWARE PACKAGE OR ANY PART THEREOF (AND THE RELATED DOCUMENTATION) FROM STMICROELECTRONICS INTERNATIONAL N.V, SWISS BRANCH AND/OR ITS AFFILIATED COMPANIES (STMICROELECTRONICS), THE RECIPIENT, ON BEHALF OF HIMSELF OR HERSELF, OR ON BEHALF OF ANY ENTITY BY WHICH SUCH RECIPIENT IS EMPLOYED AND/OR ENGAGED AGREES TO BE BOUND BY THIS SOFTWARE PACKAGE LICENSE AGREEMENT.

Under STMicroelectronics' intellectual property rights and subject to applicable licensing terms for any third-party software incorporated in this software package and applicable Open Source Terms (as defined here below), the redistribution, reproduction and use in source and binary forms of the software package or any part thereof, with or without modification, are permitted provided that the following conditions are met:
1. Redistribution of source code (modified or not) must retain any copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form, except as embedded into microcontroller or microprocessor device manufactured by or for STMicroelectronics or a software update for such device, must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of STMicroelectronics nor the names of other contributors to this software package may be used to endorse or promote products derived from this software package or part thereof without specific written permission.
4. This software package or any part thereof, including modifications and/or derivative works of this software package, must be used and execute solely and exclusively on or in combination with a microcontroller or a microprocessor devices manufactured by or for STMicroelectronics.
5. No use, reproduction or redistribution of this software package partially or totally may be done in any manner that would subject this software package to any Open Source Terms (as defined below).
6. Some portion of the software package may contain software subject to Open Source Terms (as defined below) applicable for each such portion ("Open Source Software"), as further specified in the software package. Such Open Source Software is supplied under the applicable Open Source Terms and is not subject to the terms and conditions of license hereunder. "Open Source Terms" shall mean any open source license which requires as part of distribution of software that the source code of such software is distributed therewith or otherwise made available, or open source license that substantially complies with the Open Source definition specified at www.opensource.org and any other comparable open source license such as for example GNU General Public License (GPL), Eclipse Public License (EPL), Apache Software License, BSD license and MIT license.
7. This software package may also include third party software as expressly specified in the software package subject to specific license terms from such third parties. Such third party software is supplied under such specific license terms and is not subject to the terms and conditions of license hereunder. By installing copying, downloading, accessing or otherwise using this software package, the recipient agrees to be bound by such license terms with regard to such third party software.
8. STMicroelectronics has no obligation to provide any maintenance, support or updates for the software package.
9. The software package is and will remain the exclusive property of STMicroelectronics and its licensors. The recipient will not take any action that jeopardizes STMicroelectronics and its licensors' proprietary rights or acquire any rights in the software package, except the limited rights specified hereunder.
10. The recipient shall comply with all applicable laws and regulations affecting the use of the software package or any part thereof including any applicable export control law or regulation.
11. Redistribution and use of this software package partially or any part thereof other than as permitted under this license is void and will automatically terminate your rights under this license.

THIS SOFTWARE PACKAGE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE PACKAGE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXCEPT AS EXPRESSLY PERMITTED HEREUNDER AND SUBJECT TO THE APPLICABLE LICENSING TERMS FOR ANY THIRD-PARTY SOFTWARE INCORPORATED IN THE SOFTWARE PACKAGE AND OPEN SOURCE TERMS AS APPLICABLE, NO LICENSE OR OTHER RIGHTS, WHETHER EXPRESS OR IMPLIED, ARE GRANTED UNDER ANY PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS OF STMICROELECTRONICS OR ANY THIRD PARTY.
EOF
}

# Make sure we use bash (#! may be overriden by caller script)
if [ "$(ps -o comm h -p $$)" != 'bash' ]
then
	exec /bin/bash "$0" "$@"
fi

export -f display_license

# Prompt user for license acceptance.
# Depending on options and environment, choose proper display tool.
# As terminal mode may not be detected when run from a script,
#   --force-console is here for automation purpose when testing. (ie. using expect)

set -e

box_title="STM32CubeIDE - License Agreement"

terminal_prompt() {
	rc_file=$1
	local rc

	typeset -l answer
	display_license | more
	echo
	read -p "I ACCEPT (y) / I DO NOT ACCEPT (N) [N/y] " answer
	if [ "$answer" = "y" ]; then
		# License accepted
		rc=0
		echo "License accepted."
	else
		# License not accepted
		rc=1
		echo "*** License NOT accepted. Not installing software. Hit return to exit."
		read
	fi

	# If exit code cannot be captured by caller, use this temp file
	if [ "$rc_file" ]
	then
		echo $rc > $rc_file
	fi

	exit $rc
}
export -f terminal_prompt

# Special treatment for RPM
if [[ ${BASH_SOURCE[0]} =~ '/var/tmp/rpm-tmp.' ]]; then
	if [ "$INTERACTIVE" = FALSE ] ; then
		# If not interactive and DISPLAY is not set (X11 installer seems to not propagate this variable)
		# then force it to :0
		export DISPLAY=${DISPLAY:-:0}
		# If this fails, then installation fails and user does not know it but what else can we do?
	else
		# Restore stdin as rpm installer closes it before running scriptlets.
		exec 0</dev/tty
	fi
fi

if [ -t 0 -o "$STM_FORCE_CONSOLE" ]
then
	# Terminal detected or wanted
	terminal_prompt

	# Unreached
	echo >&2 "Bug in $0 (terminal_prompt)"
	exit 3
fi

# No terminal
if [ -z "$DISPLAY" ]
then
	echo >&2 "DISPLAY not set. Cannot display license. Aborting."
	exit 2
fi


# Find first available X11 tool
dialog_tools="zenity xterm"
for tool in $dialog_tools
do
	if ( type >/dev/null -f $tool )
	then
		dialog=$tool
		break
	fi
done

case $dialog in
xterm)
	# Use terminal mode in an xterm

	# Workaround as xterm does not return "-e command" exit code
	exit_code_tmp_file=$(mktemp)
	xterm -title "$box_title" -ls -geometry 115x40 -sb -sl 1000 -e "terminal_prompt $exit_code_tmp_file"
	rc=$(cat $exit_code_tmp_file)
	rm $exit_code_tmp_file
	exit $rc
	;;
zenity)
	# Little trick below as default button of zenity is 'ok' and we want it to be 'cancel'.
	# So just swap buttons labels and use reverse condition for acceptance.
	display_license | zenity \
		--text-info \
		--title="$box_title" \
		--width=650 --height=500 \
		--cancel-label "I ACCEPT" \
		--ok-label "I DO NOT ACCEPT" \
		|| exit 0 # Accepted

	# Not accepted
	zenity \
		--error \
		--title="$box_title" \
		--text "License NOT accepted. Not installing software."
	exit 1
	;;
*)
	echo >&2 "No dialog tool found to display license. Aborting."
	exit 2
esac

# Should be unreached
echo >&2 "No way to display license. Aborting."
exit 3
