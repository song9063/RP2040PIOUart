#!/bin/bash
sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program build/rp2040-pio-uart.elf verify reset exit"