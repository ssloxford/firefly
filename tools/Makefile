all: cadu_utils ccsds_utils modis_utils
	
.PHONY: cadu_utils ccsds_utils modis_utils install
	
cadu_utils:
	$(MAKE) -C cadu_utils
	
ccsds_utils:
	$(MAKE) -C ccsds_utils

modis_utils:
	$(MAKE) -C modis_utils

install:
	$(MAKE) install -C cadu_utils
	$(MAKE) install -C ccsds_utils
	$(MAKE) install -C modis_utils
	
