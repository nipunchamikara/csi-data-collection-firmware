IDF_PATH := $(HOME)/esp/esp-idf
PORTS = /dev/tty.usbserial-0001 /dev/tty.usbserial-4 /dev/tty.usbserial-5
DEVICE := 0

get_port = $(word $(shell echo $$(($(DEVICE) + 1))),$(PORTS))

.PHONY: install build flash clean flash-monitor help

install: ## Install ESP-IDF
	$(IDF_PATH)/install.sh

build: ## Build the project
	. $(IDF_PATH)/export.sh && \
        idf.py build

run-cmd:
	. $(IDF_PATH)/export.sh && \
        idf.py -p $(call get_port,$(DEVICE)) $(CMD)

flash: ## Flash firmware to ESP32
	$(MAKE) run-cmd CMD=flash

monitor: ## Monitor the serial output
	$(MAKE) run-cmd CMD=monitor

flash-monitor: ## Flash firmware to ESP32 and monitor the serial output
	$(MAKE) run-cmd CMD="flash monitor"

clean: ## Clean the project
	. $(IDF_PATH)/export.sh && \
        idf.py clean

help: ## List all available commands
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

