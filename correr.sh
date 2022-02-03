#!/bin/bash

sed '/TestRunner summary/q' <(exec platformio test -e esp32dev) && kill -9 $!