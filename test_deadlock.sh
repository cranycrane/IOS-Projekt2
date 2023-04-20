#!/bin/bash

# Sir Bedevere skript na test deadlockov 2. IOS projektu
# Pred tým ako spustíte ten skript, naprogramujte si čakanie hlavného procesu na ukončenie všetkých podprocesov.
# !!! Tento skript neukončí čakajúce podprocesy !!!
# !!! Preto si skontrolujte 
# Spustí proj2 s danými argumentami a čaká na ukončenie hlavného procesu,
# ak neskončí do uplynutia časového limitu (time_limit) ukončí ho a vypíše DEADLOCKED.

# prepínače:
#   -b spustí len zákaldné testy
#   -l spustí testy s obrovským množstvom procesov (dobré na otestovanie zlyhania forku na merlinovi)

tests=0
report_deadlock=deadlock_test_report.txt

function deadlock_test {
    tests=$((++tests))
    echo -n "${tests}":./proj2 "$@"
    echo "${tests}":./proj2 "$@" >>${report_deadlock}
    timeout "${time_limit}" ./proj2 "$@" >/dev/null 2>>${report_deadlock}
    if [ $? == 124 ]; then
        echo -e "\t\tDEADLOCKED"
    else
        echo -e "\t\tOK"
    fi
}

basic_only=false
super_long=false
while [ $# -gt 0 ]; do
    if [ "$1" = "-l" ]; then
        super_long=true
    fi
    if [ "$1" = "-b" ]; then
        basic_only=true
    fi
    shift
done

rm -f ${report_deadlock}
make # compile

time_limit=3.0
echo "====== Start deadlock test ======"
echo " ----        basic          ---- "
deadlock_test 1 1 0 0 100
deadlock_test 3 2 100 100 100
deadlock_test 5 1 4 0 100
deadlock_test 5 2 30 0 100
deadlock_test 4 2 30 50 200
if ! ${basic_only} ; then
echo " ----      no customer      ---- "
deadlock_test 0 1 80 100 200
deadlock_test 0 50 10 100 200
deadlock_test 0 20 0 60 0
deadlock_test 0 10 9 0 0
echo " ----      no officer       ---- "
deadlock_test 3 0 20 10 100
deadlock_test 5 0 0 0 0
echo " ----       nobody          ---- "
deadlock_test 0 0 0 0 0
deadlock_test 0 0 0 0 300
echo " ----    late customers     ---- "
deadlock_test 10 5 100 30 0
deadlock_test 15 3 200 40 5
deadlock_test 10 1 800 100 300
echo " ----    instant action     ---- "
deadlock_test 8 2 0 0 0
deadlock_test 4 6 0 10 0
deadlock_test 5 2 20 0 0
deadlock_test 5 1 0 0 10
if ${super_long}; then
time_limit=10.0
echo " ---- super multi-process   ---- "
deadlock_test 100 1 50 40 1000
deadlock_test 200 1 50 40 10
deadlock_test 300 20 500 80 300
deadlock_test 30 50 300 100 500
deadlock_test 30 50 10000 100 10000
fi
fi
echo "======     Test ended      ======"
