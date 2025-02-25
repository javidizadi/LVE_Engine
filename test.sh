#!/bin/bash
make -C build test ARGS=--output-on-failure -j8
