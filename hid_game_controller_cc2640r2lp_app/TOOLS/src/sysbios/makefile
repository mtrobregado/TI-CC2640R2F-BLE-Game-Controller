
XOPTS = -I"C:/ti/xdctools_3_50_05_12_core/packages/" -Dxdc_target_types__=C:/ti/simplelink_cc2640r2_sdk_1_50_00_58/kernel/tirtos/packages/ti/targets/arm/elf/std.h -Dxdc_target_name__=M3

vpath % C:/ti/simplelink_cc2640r2_sdk_1_50_00_58/kernel/tirtos/packages/ti/sysbios/
vpath %.c C:/ti/xdctools_3_50_05_12_core/packages/

CCOPTS    = --endian=little -mv7M3 --abi=eabi -q -ms --opt_for_speed=0  --program_level_compile -o3 -g --optimize_with_debug  -Dti_sysbios_knl_Task_minimizeLatency__D=FALSE -Dti_sysbios_family_arm_cc26xx_Boot_driverlibVersion=2 -Dti_sysbios_knl_Clock_stopCheckNext__D=TRUE -Dti_sysbios_family_arm_m3_Hwi_enableException__D=TRUE -Dti_sysbios_family_arm_m3_Hwi_disablePriority__D=32U -Dti_sysbios_family_arm_m3_Hwi_numSparseInterrupts__D=0U

XDC_ROOT  = C:/ti/xdctools_3_50_05_12_core/packages/

BIOS_ROOT = C:/ti/simplelink_cc2640r2_sdk_1_50_00_58/kernel/tirtos/packages/ti/sysbios/

BIOS_DEFS =  -Dti_sysbios_BIOS_swiEnabled__D=TRUE -Dti_sysbios_BIOS_taskEnabled__D=TRUE -Dti_sysbios_BIOS_clockEnabled__D=TRUE -Dti_sysbios_BIOS_runtimeCreatesEnabled__D=TRUE -Dti_sysbios_knl_Task_moduleStateCheckFlag__D=FALSE -Dti_sysbios_knl_Task_objectCheckFlag__D=FALSE -Dti_sysbios_hal_Hwi_DISABLE_ALL_HOOKS -Dti_sysbios_knl_Swi_DISABLE_ALL_HOOKS -Dti_sysbios_BIOS_smpEnabled__D=FALSE -Dti_sysbios_Build_useHwiMacros -Dti_sysbios_Build_useIndirectReferences=FALSE -Dti_sysbios_knl_Swi_numPriorities__D=6 -Dti_sysbios_knl_Task_deleteTerminatedTasks__D=FALSE -Dti_sysbios_knl_Task_numPriorities__D=6 -Dti_sysbios_knl_Task_checkStackFlag__D=FALSE -Dti_sysbios_knl_Task_initStackFlag__D=TRUE -Dti_sysbios_knl_Task_DISABLE_ALL_HOOKS -Dti_sysbios_knl_Clock_TICK_SOURCE=ti_sysbios_knl_Clock_TickSource_TIMER -Dti_sysbios_knl_Clock_TICK_MODE=ti_sysbios_knl_Clock_TickMode_DYNAMIC -Dti_sysbios_hal_Core_delegate_getId=ti_sysbios_hal_CoreNull_getId__E -Dti_sysbios_hal_Core_delegate_interruptCore=ti_sysbios_hal_CoreNull_interruptCore__E -Dti_sysbios_hal_Core_delegate_lock=ti_sysbios_hal_CoreNull_lock__E -Dti_sysbios_hal_Core_delegate_unlock=ti_sysbios_hal_CoreNull_unlock__E -Dti_sysbios_hal_Core_numCores__D=1 -Dti_sysbios_hal_CoreNull_numCores__D=1 -Dti_sysbios_utils_Load_taskEnabled__D=TRUE -Dti_sysbios_utils_Load_swiEnabled__D=FALSE -Dti_sysbios_utils_Load_hwiEnabled__D=FALSE -Dti_sysbios_family_arm_m3_Hwi_dispatcherSwiSupport__D=TRUE -Dti_sysbios_family_arm_m3_Hwi_dispatcherTaskSupport__D=TRUE -Dti_sysbios_family_arm_m3_Hwi_dispatcherAutoNestingSupport__D=TRUE -Dti_sysbios_family_arm_m3_Hwi_dispatcherIrpTrackingSupport__D=TRUE -Dti_sysbios_knl_Semaphore_supportsEvents__D=FALSE -Dti_sysbios_knl_Semaphore_supportsPriority__D=FALSE -Dxdc_runtime_Assert_DISABLE_ALL -Dxdc_runtime_Log_DISABLE_ALL --define=BOARD_DISPLAY_USE_LCD=0 --define=BOARD_DISPLAY_USE_UART=0 --define=BOARD_DISPLAY_USE_UART_ANSI=0 --define=CC2640R2_LAUNCHXL --define=CC26XX --define=CC26XX_R2 --define=DeviceFamily_CC26X0R2 --define=Display_DISABLE_ALL --define=ICALL_EVENTS --define=ICALL_JT --define=ICALL_LITE --define=ICALL_MAX_NUM_ENTITIES=6 --define=ICALL_MAX_NUM_TASKS=4 --define=ICALL_STACK0_ADDR --define=POWER_SAVING --define=RF_SINGLEMODE --define=STACK_LIBRARY --define=USE_ICALL --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL 

BIOS_INC  = -I"C:/ti/simplelink_cc2640r2_sdk_1_50_00_58/kernel/tirtos/packages/"

INCS =  $(BIOS_INC) -I"C:/ti/simplelink_cc2640r2_sdk_1_50_00_58/source/" -I"C:/ti/simplelink_cc2640r2_sdk_1_50_00_58/kernel/tirtos/packages/" -I"C:/ti/simplelink_cc2640r2_sdk_1_50_00_58/source/ti/blestack/" -I"C:/ti/xdctools_3_50_05_12_core/packages/" -I"../" -I"/packages//" 

CC = C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl -c $(CCOPTS) -I C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include
ASM = C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl -c $(CCOPTS) -I C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include
AR = C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armar rq

DEL = C:/ti/xdctools_3_50_05_12_core/packages/../bin/rm -f
CP = C:/ti/xdctools_3_50_05_12_core/packages/../bin/cp -f

define RM
	$(if $(wildcard $1),$(DEL) $1,:)
endef

define ASSEMBLE
	@echo asmem3 $< ...
	@$(ASM) $(BIOS_DEFS) $(XOPTS) $(INCS) $<
endef

all: rom_sysbios.aem3

m3_Hwi_asm.obj: family/arm/m3/Hwi_asm.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=m3_Hwi_asm.obj

m3_Hwi_asm_switch.obj: family/arm/m3/Hwi_asm_switch.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=m3_Hwi_asm_switch.obj

m3_IntrinsicsSupport_asm.obj: family/arm/m3/IntrinsicsSupport_asm.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=m3_IntrinsicsSupport_asm.obj

m3_TaskSupport_asm.obj: family/arm/m3/TaskSupport_asm.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=m3_TaskSupport_asm.obj


rom_sysbios.obj: BIOS.c knl/Clock.c knl/Idle.c knl/Intrinsics.c knl/Event.c knl/Mailbox.c knl/Queue.c knl/Semaphore.c knl/Swi.c knl/Task.c family/arm/m3/Hwi.c family/arm/m3/TaskSupport.c family/arm/cc26xx/Boot.c family/arm/cc26xx/Timer.c family/arm/cc26xx/TimestampProvider.c hal/Hwi.c hal/Hwi_stack.c hal/Hwi_startup.c heaps/HeapMem.c heaps/HeapCallback.c gates/GateHwi.c gates/GateMutex.c xdc/runtime/xdc_noinit.c xdc/runtime/Assert.c xdc/runtime/Core-mem.c xdc/runtime/Core-smem.c xdc/runtime/Core-label.c xdc/runtime/Core-params.c xdc/runtime/Diags.c xdc/runtime/Error.c xdc/runtime/Gate.c xdc/runtime/Log.c xdc/runtime/Memory.c xdc/runtime/Registry.c xdc/runtime/Startup.c xdc/runtime/System.c xdc/runtime/SysCallback.c xdc/runtime/Text.c xdc/runtime/Timestamp.c makefile
	@-$(call RM, $@)
	@echo clem3 $< ...
	@$(CC) $(BIOS_DEFS) $(XOPTS) $(INCS) --output_file=rom_sysbios.obj \
	$(BIOS_ROOT)BIOS.c \
	$(BIOS_ROOT)knl/Clock.c \
	$(BIOS_ROOT)knl/Idle.c \
	$(BIOS_ROOT)knl/Intrinsics.c \
	$(BIOS_ROOT)knl/Event.c \
	$(BIOS_ROOT)knl/Mailbox.c \
	$(BIOS_ROOT)knl/Queue.c \
	$(BIOS_ROOT)knl/Semaphore.c \
	$(BIOS_ROOT)knl/Swi.c \
	$(BIOS_ROOT)knl/Task.c \
	$(BIOS_ROOT)family/arm/m3/Hwi.c \
	$(BIOS_ROOT)family/arm/m3/TaskSupport.c \
	$(BIOS_ROOT)family/arm/cc26xx/Boot.c \
	$(BIOS_ROOT)family/arm/cc26xx/Timer.c \
	$(BIOS_ROOT)family/arm/cc26xx/TimestampProvider.c \
	$(BIOS_ROOT)hal/Hwi.c \
	$(BIOS_ROOT)hal/Hwi_stack.c \
	$(BIOS_ROOT)hal/Hwi_startup.c \
	$(BIOS_ROOT)heaps/HeapMem.c \
	$(BIOS_ROOT)heaps/HeapCallback.c \
	$(BIOS_ROOT)gates/GateHwi.c \
	$(BIOS_ROOT)gates/GateMutex.c \
	$(XDC_ROOT)xdc/runtime/xdc_noinit.c \
	$(XDC_ROOT)xdc/runtime/Assert.c \
	$(XDC_ROOT)xdc/runtime/Core-mem.c \
	$(XDC_ROOT)xdc/runtime/Core-smem.c \
	$(XDC_ROOT)xdc/runtime/Core-label.c \
	$(XDC_ROOT)xdc/runtime/Core-params.c \
	$(XDC_ROOT)xdc/runtime/Diags.c \
	$(XDC_ROOT)xdc/runtime/Error.c \
	$(XDC_ROOT)xdc/runtime/Gate.c \
	$(XDC_ROOT)xdc/runtime/Log.c \
	$(XDC_ROOT)xdc/runtime/Memory.c \
	$(XDC_ROOT)xdc/runtime/Registry.c \
	$(XDC_ROOT)xdc/runtime/Startup.c \
	$(XDC_ROOT)xdc/runtime/System.c \
	$(XDC_ROOT)xdc/runtime/SysCallback.c \
	$(XDC_ROOT)xdc/runtime/Text.c \
	$(XDC_ROOT)xdc/runtime/Timestamp.c \

rom_sysbios.aem3: rom_sysbios.obj m3_Hwi_asm.obj m3_Hwi_asm_switch.obj m3_IntrinsicsSupport_asm.obj m3_TaskSupport_asm.obj
	@-$(call RM, $@)
	@echo arem3 $^ ...
	@$(AR) $@ $^ C:/ti/simplelink_cc2640r2_sdk_1_50_00_58/kernel/tirtos/packages/ti/sysbios/rom/cortexm/cc26xx/r2/golden/CC26xx/rom_sysbios_config.obj

clean:
	@$(DEL) ..\makefile.libs
	@-$(call RM, *)
