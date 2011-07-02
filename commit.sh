#!/bin/bash

cat config.h > config.h.last
cat extruder/config.h > extruder/config.h.last
git add -A && git commit -a && git push
