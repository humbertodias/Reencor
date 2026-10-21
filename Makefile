# Download the sprite packs listed in the README and extract them into Assets/images.
#
# Ryu and Ken archives are flat PNG dumps. They are extracted into folders named
# after the zip (02_Ryu, 11_Ken) so the game resolves keys such as "02_Ryu/03286".
# Ingame effects already contain the folders the game expects (hitsparks, fireballs,
# superart, and others); those stay nested under "22_Ingame Effects".
#
# nowak.ca is the host named in the README. justnopoint.com serves the same
# archive and is used when nowak.ca does not answer.

.PHONY: assets clean-assets run
.DELETE_ON_ERROR:

PYTHON ?= python3.12
VENV_PYTHON := .venv/bin/python

ASSETS_DIR := Assets/images
CACHE_DIR  := .cache/sprites

RYU_ZIP     := $(CACHE_DIR)/02_Ryu.zip
KEN_ZIP     := $(CACHE_DIR)/11_Ken.zip
EFFECTS_ZIP := $(CACHE_DIR)/22_Ingame_Effects.zip

RYU_DIR     := $(ASSETS_DIR)/02_Ryu
KEN_DIR     := $(ASSETS_DIR)/11_Ken
EFFECTS_DIR := $(ASSETS_DIR)/22_Ingame Effects

assets: $(RYU_DIR)/.stamp $(KEN_DIR)/.stamp $(ASSETS_DIR)/22_Ingame\ Effects/.stamp
	@echo "Sprites installed in $(ASSETS_DIR)"

$(CACHE_DIR):
	mkdir -p $@

$(RYU_ZIP): URLS := https://www.justnopoint.com/zweifuss/all/02_Ryu.zip
$(KEN_ZIP): URLS := https://www.justnopoint.com/zweifuss/all/11_Ken.zip
$(EFFECTS_ZIP): URLS := https://www.justnopoint.com/zweifuss/all/22_Ingame%20Effects.zip

$(RYU_ZIP) $(KEN_ZIP) $(EFFECTS_ZIP): | $(CACHE_DIR)
	@set -e; \
	rm -f "$@.partial"; \
	ok=0; \
	for url in $(URLS); do \
		echo "Downloading $$url"; \
		if curl -fL --connect-timeout 8 --max-time 180 \
			-o "$@.partial" "$$url"; then \
			ok=1; \
			break; \
		fi; \
		echo "Failed: $$url"; \
		rm -f "$@.partial"; \
	done; \
	test "$$ok" = 1; \
	mv "$@.partial" "$@"

$(RYU_DIR)/.stamp: $(RYU_ZIP)
	mkdir -p $(RYU_DIR)
	unzip -q -o $< -d $(RYU_DIR)
	test -f $(RYU_DIR)/03286.png
	touch $@

$(KEN_DIR)/.stamp: $(KEN_ZIP)
	mkdir -p $(KEN_DIR)
	unzip -q -o $< -d $(KEN_DIR)
	test -f $(KEN_DIR)/14657.png
	touch $@

$(ASSETS_DIR)/22_Ingame\ Effects/.stamp: $(EFFECTS_ZIP)
	mkdir -p "$(EFFECTS_DIR)"
	unzip -q -o $< -d "$(EFFECTS_DIR)"
	test -f "$(EFFECTS_DIR)/hitsparks/29846.png"
	test -f "$(EFFECTS_DIR)/fireballs/30648.png"
	touch "$@"

clean-assets:
	rm -rf $(RYU_DIR) $(KEN_DIR) "$(EFFECTS_DIR)" $(CACHE_DIR)

$(VENV_PYTHON):
	$(PYTHON) -m venv .venv
	$(VENV_PYTHON) -m pip install -U pip
	$(VENV_PYTHON) -m pip install pygame PyOpenGL

run: $(VENV_PYTHON) assets
	$(VENV_PYTHON) main.py
