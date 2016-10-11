
.PHONY : help compilation archive 

SHELL := /bin/bash

##
## HELP
##

help : 
	@echo "Available:"
	@echo "- compilation ==> compilation (should not produce any error nor warning)"
	@echo "- test        ==> $(T_TEST__LIST) TV* TO*"
	@echo "  - t_sstring => make test with ./test_sstring"
	@echo "  - m_sstring => valgrind ./test_sstring"
	@echo "  - t_term    => make test with ./test_term"
	@echo "  - m_term    => valgrind ./test_term"
	@echo "  - t_variable    => make test with ./test_variable"
	@echo "  - m_variable    => valgrind ./test_vatiable"
	@echo "  - TR% (% is a number) => test rewrite output on t_rerwite_%.term"
	@echo "  - TR => test rewrite output on all t_rerwite_%.term"
	@echo "  - MR% (% is a number) => test rewrite memory on t_rerwite_%.term"
	@echo "  - MR => test rewrite memory on all t_rerwite_%.term"
	@echo "  - TU% TU MU% MU => test on unify"
	@echo "  - TV% TV MV% MV => test on valuate"
	@echo "  - T => all test on output"
	@echo "  - M => all test on memory"
# unify valuate
	@echo "- archive     => produce the tgz archive"


##
## MODULES
##

MODULE := sstring term term_io term_variable valuate unify rewrite


##
## HEADER FILES
##

HEADERS := $(MODULE:%=%.h)


##
## TERMS
##

TEST_PROGRAM := test_sstring test_term test_variable test_rewrite test_valuate test_unify


##
##  COMPILATION
##

## Create modules and test terms
compilation : $(MODULE:%=%.o) $(TEST_PROGRAM)

## Compiler

CC := gcc

## Compilation options

C_FLAG_OFF_UNUSED := -Wno-unused-but-set-parameter -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-abi
# de-activate noisy warnings

CFLAGS := -std=c99 -Wall -Wextra -pedantic -ggdb -lm $(C_FLAG_OFF_UNUSED)

## compilation rules

./%.o : %.c $(HEADERS) 
	$(CC) -c $(CFLAGS) -o $@ $<

./% : %.c $(MODULE:%=%.o) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(MODULE:%=%.o) $*.c


##
## TEST
##

## Directory of for all data and results
DATA_DIR := DATA

## Directory of terms 
TERM_DIR := $(DATA_DIR)/Terms

## Directory to put generated results
RESULTS_DIR := $(DATA_DIR)/Results

## Directory of expected results_dir
RESULTS_EXPECTED_DIR := $(DATA_DIR)/Results_Expected

## Memory checker
VALGRIND := valgrind --leak-check=full --show-leak-kinds=all >/dev/null

## Valgrind returns when there is no leak
VALGRIND_INVALID_READ := "Invalid read"
VALGRIND_NO_MEMORY_LEAK_MESSAGE := "All heap blocks were freed -- no leaks are possible"


## To 1) run 2) compare with expected 
define TEST_T
	@echo "==OUTPUT================ $(1) ====================="
	$(1) > $(RESULTS_DIR)/$(2).output
	@echo diff -Z $(RESULTS_DIR)/$(2).output $(RESULTS_EXPECTED_DIR)/$(2).output
	@if ! diff -Z $(RESULTS_DIR)/$(2).output $(RESULTS_EXPECTED_DIR)/$(2).output ; then echo "$(1): *** RÉSUTALT INCORRECT ***" ; false ; else echo "$(1) on $(2): outputs match -- OK" ; fi
endef

## To 1) test memorey leaks
define TEST_M
	@echo "==VALGRIND============== $(1) ====================="
	@if ( ( ! ( 2>&1 $(VALGRIND) $(1) | tee $(RESULTS_DIR)/$(2).valgrind_output | grep $(VALGRIND_NO_MEMORY_LEAK_MESSAGE) ) ) || ( grep $(VALGRIND_INVALID_READ) $(RESULTS_DIR)/$(2).valgrind_output  ) ) ; then cat $(RESULTS_DIR)/$(2).valgrind_output ; false ; fi
endef



TR% : ./test_rewrite
	$(call TEST_T,./test_rewrite < $(TERM_DIR)/t_rewrite_$*.term,t_rewrite_$*.term)
TU% : ./test_unify
	$(call TEST_T,./test_unify < $(TERM_DIR)/t_unify_$*.term,t_unify_$*.term)
TV% : ./test_valuate
	$(call TEST_T,./test_valuate < $(TERM_DIR)/t_valuate_$*.term,t_valuate_$*.term)

MR% : ./test_rewrite
	$(call TEST_M,./test_rewrite < $(TERM_DIR)/t_rewrite_$*.term,t_rewrite_$*.term)
MU% : ./test_unify
	$(call TEST_M,./test_unify < $(TERM_DIR)/t_unify_$*.term,t_unify_$*.term)
MV% : ./test_valuate
	$(call TEST_M,./test_valuate < $(TERM_DIR)/t_valuate_$*.term,t_valuate_$*.term)

TERM_R_NUMBERS = $(sort $(subst .term,,$(subst $(TERM_DIR)/t_rewrite_,,$(wildcard $(TERM_DIR)/t_rewrite_*.term))))
TERM_U_NUMBERS = $(sort $(subst .term,,$(subst $(TERM_DIR)/t_unify_,,$(wildcard $(TERM_DIR)/t_unify_*.term))))
TERM_V_NUMBERS = $(sort $(subst .term,,$(subst $(TERM_DIR)/t_valuate_,,$(wildcard $(TERM_DIR)/t_valuate_*.term))))

.PHONY : TR MR TU MU TV MV T M

TR : $(TERM_R_NUMBERS:%=TR%)
MR : $(TERM_R_NUMBERS:%=MR%)
TU : $(TERM_U_NUMBERS:%=TU%)
MU : $(TERM_U_NUMBERS:%=MU%)
TV : $(TERM_V_NUMBERS:%=TV%)
MV : $(TERM_V_NUMBERS:%=MV%)

## TEST output sstring term validate…
t_% : ./test_%
	$(call TEST_T,./test_$*,test_$*)


## TEST memory sstring term validate…
m_% : ./test_%
	$(call TEST_M,./test_$*,test_$*)


## TEST basic
t_test : t_sstring t_term t_variable

m_test : m_sstring m_term m_variable

T : t_test TR TU TV
M : m_test MR MU MV


##
## PRODUCE THE ARCHIVE (to umploaded on Celene)
## 

ARCHIVE_NAME := PASD_mini-projet.tgz
ARCHIVE_FILES := Makefile *.c *.h compte-rendu.pdf

# à compléter si pour inclure d'autres fichiers
ARCHIVE_OTHER_FILES :=


archive :
	@tar czf $(ARCHIVE_NAME) $(ARCHIVE_FILES)


