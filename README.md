# Amplifier Driver for Texas Instruments tas2557 Class-D Audio Amplifier.

## Audfilter
  - Upper Filter of Audminiport driver.
  - Use this driver to filter IoControlCode and notify tas driver unmute.
  - **Please Make Sure "UpperFilters" was set in audminiport_basexxx.inf** like this
    ```
    [AUDIOHW.AddReg]
    ; ...ignore...
    HKR,,"UpperFilters",%REG_MULTI_SZ%,"AudFilter"
    ```
## tas2557_amp
  - Auto-generated codes to setup fw/calibration/startup/unmute sequence for TAS2557.
  - Notice:
    + The Configuration choosing here is `configuration_Tuning Mode_48 KHz_s1_0`

## Reference Codes
  - CS35l41_win and other amp drivers for windows.
  - MS sample spb.c and sample driver.

## Known Issues
  - Spkrs not shutdown and keep producing noise
    + Solution: reboot.
  - Spkrs work sometimes, and not work sometimes.
  - Sometimes the noise in spkr is very loud while playing music.
    + Solution: stop any audio source, wait 10 seconds, playback again.

## ACPI Sample code
```
    Device(AFT1){
        Name (_HID, "AFLT0001")
        Name (_UID, Zero)  // _UID: Unique ID
        Name (_DEP, Package ()  // _DEP: Dependencies
        {
            \_SB.ADSP.SLM1.ADCM.AUDD
        })
    }
    
    Device (SPK1)
    {
        Name (_HID, "TTAS2557") // Taxes TAS2557
        Name (_UID, 0)
        Alias(\_SB.PSUB, _SUB)
        Name (_DEP, Package()
        {
            \_SB.GIO0, 
            \_SB.I2C6
        })
        Method (_CRS, 0x0, NotSerialized)
        {
            Name (RBUF, ResourceTemplate()
            {
                I2CSerialBus(0x4C, ,400000, AddressingMode7Bit, "\\_SB.I2C6",,,,)
                GpioInt(Level, ActiveHigh, Exclusive, PullDown, 0, "\\_SB.GIO0") {30}
            })
            Return (RBUF)
        }
    }
```