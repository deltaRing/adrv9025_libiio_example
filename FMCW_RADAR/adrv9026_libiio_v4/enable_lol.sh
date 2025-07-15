#!/bin/bash
# Enable TX1 and TX2 LO Leakage Tracking Calibration on ADRV9026

# Replace with your actual IIO device path (check with iio_info if unsure)
DEVICE_PATH="/sys/bus/iio/devices/iio:device2"

echo "Enabling LO Leakage Tracking Calibration for TX1..."
echo 1 > $DEVICE_PATH/out_voltage0_lo_leakage_tracking_en

echo "Enabling LO Leakage Tracking Calibration for TX2..."
echo 1 > $DEVICE_PATH/out_voltage1_lo_leakage_tracking_en

echo "LO Leakage Tracking Calibration enabled."
