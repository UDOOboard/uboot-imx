#!/bin/bash

type_Qseven="QSEVEN"
type_uQseven="UQSEVEN"
type_uSBC="uSBC"
type_A62="A62"

type_cpu_q="QUAD"
type_cpu_dl="DUAL_LITE"
type_cpu_s="SOLO"

CONFIG_FILE="./tools/sconfig"

env_MMC="MMC"
env_SATA="SATA"
env_SPI="SPI"

BACKTITLE='U-boot 2014 SECO config'

SELECTION=""
SEL_ITEM=1
SELECTION_COMP=""
SUBSEL=""
EXIT_RESPONCE=0
EXIT=0

# Default values
MEM_SIZE=3
BOARD=${type_Qseven}
CPU_TYPE=${type_cpu_q}
ENV_DEV=${env_MMC}
CLEAN="CLEAN"
EXTRA_UART=""
DEBUG_UART5=""
RTC_LOW_POWER=""
COMPILER_PATH="/opt/compilers/arm-poky-linux-gnueabi_4.9.1/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-"
UBOOT_VER="2013"

SUFFIX=""

#################################################################
#																#
#					CONFIG FILE FUNCTION						#
#																#
#################################################################

set_ConfFile() {
	echo "MEMORY_SIZE $MEM_SIZE" > $CONFIG_FILE
	echo "BOARD_TYPE $BOARD" >> $CONFIG_FILE
	echo "CPU_TYPE $CPU_TYPE" >> $CONFIG_FILE
	echo "ENV_DEVICE $ENV_DEV" >> $CONFIG_FILE
	echo "CLEAN_OP $CLEAN" >> $CONFIG_FILE
	echo "EXTRA_UART $EXTRA_UART" >> $CONFIG_FILE
	echo "DEBUG_UART5 $DEBUG_UART5" >> $CONFIG_FILE
	echo "RTC_LOW_POWER $RTC_LOW_POWER" >> $CONFIG_FILE
	echo "RTC_EXT $RTC_EXT" >> $CONFIG_FILE
	echo "COMPILER $COMPILER_PATH" >> $CONFIG_FILE
	echo "UBOOT_VER $UBOOT_VER" >> $CONFIG_FILE
}

set_from_ConfFile() {
	if [[ -e $CONFIG_FILE ]]; then
		VAR=$(cat $CONFIG_FILE | grep "MEMORY_SIZE" | awk '{print $2}')
		if [[ "${VAR}" != "" ]]; then
			MEM_SIZE=$VAR
		fi
		VAR=$(cat $CONFIG_FILE | grep "BOARD_TYPE" | awk '{print $2}')
		if [[ "${VAR}" != "" ]]; then
			BOARD=$VAR
		fi
		VAR=$(cat $CONFIG_FILE | grep "CPU_TYPE" | awk '{print $2}')
		if [[ "${VAR}" != "" ]]; then
			CPU_TYPE=$VAR
		fi
		VAR=$(cat $CONFIG_FILE | grep "ENV_DEVICE" | awk '{print $2}')
		if [[ "${VAR}" != "" ]]; then
			ENV_DEV=$VAR
		fi
		VAR=$(cat $CONFIG_FILE | grep "CLEAN_OP" | awk '{print $2}')
		if [[ "${VAR}" != "" ]]; then
			CLEAN=$VAR
		fi
		VAR=$(cat $CONFIG_FILE | grep "EXTRA_UART" | awk '{print $2}')
		if [[ "${VAR}" != "" ]]; then
			EXTRA_UART=$VAR
		fi
		VAR=$(cat $CONFIG_FILE | grep "DEBUG_UART5" | awk '{print $2}')
                if [[ "${VAR}" != "" ]]; then
                        DEBUG_UART5=$VAR
                fi
		VAR=$(cat $CONFIG_FILE | grep "RTC_LOW_POWER" | awk '{print $2}')
		if [[ "${VAR}" != "" ]]; then
			RTC_LOW_POWER=$VAR

		fi
		VAR=$(cat $CONFIG_FILE | grep "RTC_EXT" | awk '{print $2}')
		if [[ "${VAR}" != "" ]]; then
		RTC_EXT=$VAR

		fi
		VAR=$(cat $CONFIG_FILE | grep "COMPILER" | awk '{print $2}')
		if [[ "${VAR}" != "" ]]; then
			COMPILER_PATH=$VAR
		fi
		VAR=$(cat $CONFIG_FILE | grep "UBOOT_VER" | awk '{print $2}')
		if [[ "${VAR}" != "" ]]; then
			UBOOT_VER=$VAR
		fi
	else
		echo "WARNING: Configuration file not found!"
		set_ConfFile
	fi
}


#################################################################
#																#
#						GRAPHIC FUNCTION						#
#																#
#################################################################


main_view() {
	# open fd
	exec 3>&1
	 
	# Store data to $VALUES variable
	SELECTION=$(dialog --title "Main Menu" \
			--backtitle "$BACKTITLE" \
			--ok-label "Select" \
			--default-item $SEL_ITEM \
			--cancel-label "Exit" \
			--menu "Please choose an operation:" 25 60 10 \
			1 "DDR Size -->" \
			2 "Board Type -->" \
			3 "CPU type -->" \
			4 "Environment device -->" \
			5 "Extra options -->" \
			6 "Compiler options -->" \
			2>&1 1>&3)
	 
	# close fd
	exec 3>&-
	
	if [[ "${SELECTION}" == "" ]]; then
		EXIT=1
 	fi
	# display values just entered
#	echo "$SELECTION"
	
}

ddr_size_view() {
	# open fd
	exec 3>&1
	VAL=(off off off off off)
	VAL[$MEM_SIZE]=on
	# Store data to $VALUES variable
	SELECTION=$(dialog --title "DDR configuration" \
			--backtitle "$BACKTITLE" \
			--ok-label "Select" \
			--cancel-label "Exit" \
			--default-item $MEM_SIZE \
			--radiolist "Select DDR type:" 20 80 10 \
			0 "512M,  bus size 32, active CS = 1 (256Mx2)" ${VAL[0]} \
 			1 "1Giga, bus size 32, active CS = 1 (512Mx2)" ${VAL[1]} \
			2 "1Giga, bus size 64, active CS = 1 (256Mx4)" ${VAL[2]} \
			3 "2Giga, bus size 64, active CS = 1 (512Mx4)" ${VAL[3]} \
 			4 "4Giga, bus size 64, active CS = 2 (512Mx8) - for QSEVEN only" ${VAL[4]} \
			2>&1 1>&3)
	 
	# close fd
	exec 3>&-
	
	if [[ "${SELECTION}" == "" ]]; then
		echo "not select"
	else
		MEM_SIZE=$SELECTION
	fi	
}

board_type_view() {
	# open fd
	exec 3>&1
	VAL=(off off off off)
	case "$BOARD" in
			"$type_Qseven") VAL[0]=on;;
		   	"$type_uQseven") VAL[1]=on;;
			"$type_uSBC") VAL[2]=on;;
			"$type_A62") VAL[3]=on;;
	esac
	# Store data to $VALUES variable
	SELECTION=$(dialog --title "Board type" \
			--backtitle "$BACKTITLE" \
			--ok-label "Ok" \
			--cancel-label "Exit" \
			--default-item $BOARD \
			--radiolist "Select Board type:" 20 60 10 \
			$type_Qseven "Qseven RevB board" ${VAL[0]} \
			$type_uQseven "Micro Qseven board" ${VAL[1]} \
			$type_uSBC "Micro Single board" ${VAL[2]} \
			$type_A62 "A62 Single board" ${VAL[3]} \
			2>&1 1>&3)
	 
	# close fd
	exec 3>&-
	 
	if [[ "${SELECTION}" == "" ]]; then
		echo "not select"
	else
		BOARD=$SELECTION
	fi	
}

cpu_type_view() {
	# open fd
	exec 3>&1
	VAL=(off off off)
	case "$CPU_TYPE" in
			"$type_cpu_q") VAL[0]=on;;
			"$type_cpu_dl") VAL[1]=on;;
			"$type_cpu_s") VAL[2]=on;;
	esac
	# Store data to $VALUES variable
	SELECTION=$(dialog --title "CPU type" \
			--backtitle "$BACKTITLE" \
			--ok-label "Ok" \
			--cancel-label "Exit" \
			--radiolist "Select CPU type:" 20 60 10 \
			$type_cpu_q "iMX6 QUAD" ${VAL[0]} \
			$type_cpu_dl "iMX6 DUAL LITE" ${VAL[1]} \
 			$type_cpu_s "iMX6 SOLO" ${VAL[2]} \
			2>&1 1>&3)
	 
	# close fd
	exec 3>&-
	 
	if [[ "${SELECTION}" == "" ]]; then
		echo "not select"
	else
		CPU_TYPE=$SELECTION
	fi	
}

env_dev_view() {
	# open fd
	exec 3>&1
	VAL=(off off off)
	case "$ENV_DEV" in
		 "${env_MMC}") VAL[0]=on;; 
		"${env_SATA}") VAL[1]=on;;
		 "${env_SPI}") VAL[2]=on;;
	esac
	# Store data to $VALUES variable
	SELECTION=$(dialog --title "Environment device" \
			--backtitle "$BACKTITLE" \
			--ok-label "Ok" \
			--cancel-label "Exit" \
			--default-item $ENV_DEV \
			--radiolist "Select device for environmnet storing:" 20 60 10 \
			$env_MMC "SD/MMC as environment device"  ${VAL[0]} \
 			$env_SATA "SATA as environment device"  ${VAL[1]} \
			$env_SPI "SPI as enviroment device"  ${VAL[2]} \
			2>&1 1>&3)
	 
	# close fd
	exec 3>&-
	
	if [[ "${SELECTION}" == "" ]]; then
		echo "not select"
	else
		ENV_DEV=$SELECTION
	fi	
}

extra_view() {
	# open fd
	exec 4>&1
	VAL=(off off off off off)
	if [[ "${CLEAN}" == "CLEAN" ]]; then
		VAL[0]=on   
	fi
	if [[ "${EXTRA_UART}" == "EXTRA_UART" ]]; then
		VAL[1]=on
	fi
	if [[ "${DEBUG_UART5}" == "DEBUG_UART5" ]]; then
                VAL[2]=on
    fi
	if [[ "${RTC_LOW_POWER}" == "RTC_LOW_POWER" ]]; then
                VAL[3]=on
        fi
	if [[ "${RTC_EXT}" == "RTC_EXT" ]]; then
                VAL[4]=on
        fi
	# Store data to $VALUES variable
	SELECTION=$(dialog --title "Extra option" \
			--backtitle "$BACKTITLE" \
			--ok-label "Ok" \
			--cancel-label "Exit" \
			--checklist "General settings:" 20 60 10 \
			CLEAN "Clear befor compile" ${VAL[0]} \
			EXTRA_UART "Use an addition UART for a basic comunication" ${VAL[1]} \
			DEBUG_UART5 "Use UART5 as Serial Debug (only for 984 RevC Board)" ${VAL[2]} \
			RTC_LOW_POWER "Use low power RTC" ${VAL[3]} \
			RTC_EXT "Use external low power RTC" ${VAL[4]} \
			2>&1 1>&4)
	 
	# close fd
	exec 4>&-
	if [[ "${SELECTION}" == "" ]]; then
		echo "not select"
		CLEAN="NOCLEAN"
	else
	        EXTRA_UART=""
		DEBUG_UART5=""
		RTC_LOW_POWER=""
		RTC_EXT=""
		str=${SELECTION//\"/""}
		IFS=' ' read -a array <<< "$str"
		for i in ${array[*]}; do
			if [[ "$i" == "CLEAN" ]]; then
				CLEAN="${i}"	
			fi
			if [[ "$i" == "EXTRA_UART" ]]; then
				EXTRA_UART="${i}"
			fi
			if [[ "$i" == "DEBUG_UART5" ]]; then
                                DEBUG_UART5="${i}"
                        fi
			if [[ "$i" == "RTC_LOW_POWER" ]]; then
                                RTC_LOW_POWER="${i}"
                        fi
			if [[ "$i" == "RTC_EXT" ]]; then
                                RTC_EXT="${i}"
                        fi
		done	
	fi	
}

compile_view() {
	EXIT_COMP=0
	while [[ $EXIT_COMP -ne 1 ]]; do
		# open fd
		exec 3>&1
		
		SELECTION_COMP=$(dialog --title "Compile options" \
				--backtitle "$BACKTITLE" \
				--ok-label "Ok" \
				--cancel-label "Cancel" \
				--backtitle "$BACKTITLE" \
				--ok-label "Select" \
				--cancel-label "Exit" \
				--menu "Please choose an operation:" 25 60 10 \
				1 "Compiler path" \
				2 "u-boot version" \
				2>&1 1>&3)	

		case "$SELECTION_COMP" in 
			"1") SUBSEL=$(dialog --title "" \
					--backtitle "$BACKTITLE" \
					--nocancel \
					--inputbox "Enter Cross Compiler path here" 8 60 "$COMPILER_PATH" \
					2>&1 1>&3)
					if [[ "${SUBSEL}" != "" ]]; then
						COMPILER_PATH=$SUBSEL		
					fi;;	
			"2") SUBSEL=$(dialog --title "" \
					--backtitle "$BACKTITLE" \
					--nocancel \
					--inputbox "Enter u-boot version here" 8 60 "$UBOOT_VER" \
					2>&1 1>&3)
					if [[ "${SUBSEL}" != "" ]]; then
						UBOOT_VER=$SUBSEL		
					fi;;
			  *) EXIT_COMP=1
		esac
	
		# close fd
		exec 3>&-	

	done
}

function exit_view () {
	dialog --title "" \
			--backtitle "$BACKTITLE" \
			--extra-button \
			--extra-label "Yes, with Compile" \
			--cancel-label "No" \
			--yesno "Want to save before exiting?" 5 70
	EXIT_RESPONCE=$?
}

#################################################################
#																#
#						COMPILE FUNCTION						#
#																#
#################################################################

function check_mem_size () {
	echo ""
	if [ "${MEM_SIZE}" == "0" ]; then
		echo "RAM size selected: 512M,  bus size 32, active CS = 1 (256Mx2)"
		SUFFIX=${SUFFIX}-512MB
	elif [ "${MEM_SIZE}" == "1" ]; then
		echo "RAM size selected: 1Giga, bus size 32, active CS = 1 (512Mx2)"
		SUFFIX=${SUFFIX}-512MBx2
	elif [ "${MEM_SIZE}" == "2" ]; then
		echo "RAM size selected: 1Giga, bus size 64, active CS = 1 (256Mx4)"
		SUFFIX=${SUFFIX}-256MBx4
	elif [ "${MEM_SIZE}" == "3" ]; then
		echo "RAM size selected: 2Giga, bus size 64, active CS = 1 (512Mx4)"
		SUFFIX=${SUFFIX}-2GB
	elif [ "${MEM_SIZE}" == "4" ]; then
		echo "RAM size selected: 4Giga, bus size 64, active CS = 2 (512Mx8) - for QSEVEN only"
		SUFFIX=${SUFFIX}-4GB
	else
		echo "ERROR: wrong DDR size selected"
		exit 0
	fi
}

function check_board_type () {
	echo ""
	if [ "${BOARD}" == "${type_Qseven}" ]; then
		echo "Board type selected: Qseven module"
		SUFFIX=${SUFFIX}-Q7
	elif [ "${BOARD}" == "${type_uQseven}" ]; then
		echo "Board type selected: micro Qseven module"
		SUFFIX=${SUFFIX}-uQ7
	elif [ "${BOARD}" == "${type_uSBC}" ]; then
                echo "Board type selected: Micro Single Board"
                SUFFIX=${SUFFIX}-uSBC
	elif [ "${BOARD}" == "${type_A62}" ]; then
                echo "Board type selected: A62 Single Board"
                SUFFIX=${SUFFIX}-a62
	else
		echo "ERROR: wrong board type selected"
		exit 0
	fi
}

function check_cpu_type () {
	echo ""
    if [ "${CPU_TYPE}" == "${type_cpu_q}" ]; then
        echo "make mx6q_seco_config"
		SUFFIX=${SUFFIX}-QD
    elif [ "${CPU_TYPE}" == "${type_cpu_dl}" ]; then
        echo "make mx6dl_seco_config"
		SUFFIX=${SUFFIX}-DL
    elif [ "${CPU_TYPE}" == "${type_cpu_s}" ]; then
        echo "make mx6solo_seco_config"
		SUFFIX=${SUFFIX}-S
	else
		echo "ERROR: No CPU Type selected "
	fi
}
		
function check_env_device_type () {
	echo ""
	if [ "${ENV_DEV}" == "${env_MMC}" ]; then
		echo "Environment selected: MMC"
	elif [ "${ENV_DEV}" == "${env_SATA}" ]; then
		echo "Environment selected: SATA"
	elif [ "${ENV_DEV}" == "${env_SPI}" ]; then
		echo "Environment selected: SPI"
	else
		echo "ERROR: wrong environment selected"
		exit 0
	fi
}

function make_cpu_type () {
	echo ""
	if [ "${BOARD}" == "${type_Qseven}" ]; then
	    if [ "${CPU_TYPE}" == "${type_cpu_q}" ]; then
	    	  make mx6q_quadmo747_928_config   
	    elif [ "${CPU_TYPE}" == "${type_cpu_dl}" ]; then
	          make mx6dl_quadmo747_928_config
	    elif [ "${CPU_TYPE}" == "${type_cpu_s}" ]; then
	          make mx6s_quadmo747_928_config
	    fi 
	elif [ "${BOARD}" == "${type_uQseven}" ]; then
	    if [ "${CPU_TYPE}" == "${type_cpu_q}" ]; then
	        make mx6q_uq7_962_config
	    elif [ "${CPU_TYPE}" == "${type_cpu_dl}" ]; then
	        make mx6dl_uq7_962_config
	    elif [ "${CPU_TYPE}" == "${type_cpu_s}" ]; then
	        make mx6s_uq7_962_config
	    fi
	elif [ "${BOARD}" == "${type_A62}" ]; then
            if [ "${CPU_TYPE}" == "${type_cpu_q}" ]; then
                make mx6q_SBC_A62_config
            elif [ "${CPU_TYPE}" == "${type_cpu_dl}" ]; then
		make mx6dl_SBC_A62_config
            elif [ "${CPU_TYPE}" == "${type_cpu_s}" ]; then
                make mx6s_SBC_A62_config
            fi
        fi
}

function compile () {
	export ARCH=arm
	export CROSS_COMPILE=$COMPILER_PATH

	SUFFIX=""	
	#N.B. don't change this calling order
	check_board_type	
	check_cpu_type
	check_mem_size	
	check_env_device_type

	if [ "${EXTRA_UART}" == "EXTRA_UART" ]; then
		echo ""
		echo "Select the use of the EXTRA_UART"
		echo ""
	fi
	
	if [ "${DEBUG_UART5}" == "DEBUG_UART5" ]; then
                echo ""
                echo "Select the use of the DEBUG_UART5"
                echo ""
        fi

	if [ "${CLEAN}" == "CLEAN" ]; then
		echo ""
		echo "Select Clean operation!"
		echo ""
		make distclean
		sleep 1
	fi
	
	if [ "${RTC_LOW_POWER}" == "RTC_LOW_POWER" ]; then
                echo ""
                echo "Select the use of the Low Power RTC"
                echo ""
        fi
	if [ "${RTC_EXT}" == "RTC_EXT" ]; then
                echo ""
                echo "Select the use of the external Low Power RTC"
                echo ""
        fi
	
	
	make_cpu_type
	
	SUFFIX=${SUFFIX}-${UBOOT_VER}
	echo ${SUFFIX} > ./tools/suffix
	sleep 2

	echo ---- ${MEM_SIZE}
        make -j8 DDR_SIZE=${MEM_SIZE} 
#	cp u-boot.imx u-boot$(cat ./tools/suffix | awk '{print $1}')$(./tools/setlocalversion ./).imx
}

#################################################################
#																#
#																#
#################################################################

function help_view () {
	echo "SECO U-boot compiler"
	echo "Usage: $0 [-c for configuration option]"
	echo
}

set_from_ConfFile
while getopts ":m:b:p:dch" optname; do
	case "$optname" in
		"c") while [[ $EXIT -ne 1 ]]; do
				main_view
				SEL_ITEM=$SELECTION
				case "$SELECTION" in
					"1") ddr_size_view;;
					"2") board_type_view;;
					"3") cpu_type_view;;
					"4") env_dev_view;;
					"5") extra_view;;
					"6") compile_view;;
					  *) echo "" ;;
				esac
			done

			exit_view
			case "${EXIT_RESPONCE}" in
				  "0") set_ConfFile; clear; echo "Configuration saved!";;
				  "1") clear; echo "Configuration not saved!";;
				  "3") set_ConfFile; clear; echo "Configuration saved!"; compile;;
				"255") clear;  echo "Configuration not saved!";;
					*) clear;
			esac
			exit 0;;

		  h) help_view
			 exit 0;;	
		  *) echo "ERROR: option not valid!"
			 help_view
			 exit 1;;
	esac
done

#if no any option is present, the compilation start directly
compile

