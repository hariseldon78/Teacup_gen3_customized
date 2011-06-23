#!/bin/bash

cat config.h > config.h.last
git add -A && git commit -a && git push
