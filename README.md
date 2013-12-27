Wireless Buzzers
================

## Description
This repository serves as the home for the firmware and protocol description of the wireless buzzer system.

## Repository Layout
firmware/ - contains buzzer and master firmware

firmware/buzzer/ - contains firmware for the player's buzzers

firmware/master/ - contains firmware for the master unit

## Protocol Description
1. On button press, the buzzer will wake from sleep and send eight packets of one byte each containing its unique BUZZER_ID to the master address. Any buzzer that is not pressed will not wake from sleep, and it will therefore ignore all communications until it is woken up.
2. The buzzer will switch to receive mode and wait up to 30 seconds for a response. If a response is not received inside the 30 second window, the buzzer will return to sleep. The master unit will receive a packet from the first player to press their button and ignore all further incoming packets. The master unit will wait a reasonable period of time and then send ACK packets on the broadcast address, each containing one byte, the BUZZER_ID of the buzzer to win the buzzer race.
3. Each buzzer that is not asleep and receives an ACK packet will compare the BUZZER_ID in the packet to its own, and if different, will return to sleep. If the BUZZER_ID matches, then the buzzer will light its LED to alert the player that they have won the buzzer race.
4. Buzzers that are still awake will again wait 30 seconds for a RESET packet. Once ACK packets have been sent, the master begins an automatic five second quizbowl countdown timer, sounds a piezo buzzer, and displays the number and team of the player to win the buzzer race. When the controlling supervisor presses the reset button on the master unit, the master will send a series of RESET packets containing the byte 0x01. Any buzzer that receives this packet will ensure that its LED is off and then return to sleep, awaiting the next protocol invocation.

Any buzzer that times out waiting for a response will ensure its LED is not lit and return to sleep so as to save battery in the case that the master is not powered on or the rare situation that all eight packets sent are dropped due to extreme interference.
