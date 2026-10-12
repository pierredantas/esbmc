#!/bin/bash
while read -r line; do case "$line" in *check-sat*) echo '(error "boom")';; esac; done
