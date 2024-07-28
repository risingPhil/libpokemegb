# Filter out the unwanted MAKEFLAGS
.PHONY: all clean

all:
	$(MAKE) -C do_stuff_gen1 $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C do_stuff_gen2 $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C encodeText_gen1 $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C encodeText_gen2 $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C decodeSprite $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C addDistributionPoke $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C gen2_gsball $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C gen2_addItem $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C gen2_removeItem $(MAKECMDGOALS) $(MAKEOVERRIDES)

clean:
	$(MAKE) -C do_stuff_gen1 clean
	$(MAKE) -C do_stuff_gen2 clean
	$(MAKE) -C encodeText_gen1 clean
	$(MAKE) -C encodeText_gen2 clean
	$(MAKE) -C decodeSprite clean
	$(MAKE) -C addDistributionPoke clean
	$(MAKE) -C gen2_gsball clean
	$(MAKE) -C gen2_addItem clean
	$(MAKE) -C gen2_removeItem clean
