#!/bin/sh

MAX_PATH=10000000
MEMLIM=1024

FLAGS="-DCOLLAPSE -D_POSIX_SOURCE -DMEMLIM=${MEMLIM} -DXUSAFE"

LATEX_FILE=result.tex
rm -f $LATEX_FILE

verif()
{
    spin -a -X "$1"
    gcc -w -o verif  -DSAFETY -DNOCLAIM $FLAGS pan.c
    /usr/bin/time -f "veriftime :%U:" -o ressources ./verif -n -E #-m${MAX_PATH}
    rm pan.c
}

pFile()
{
    echo "$1" >> $LATEX_FILE
}

startTab()
{
NUMS="$1"

entete="\begin{tabular}{|l"

for numModules in $NUMS
do
    entete="$entete |c |c"
done
entete="$entete |}"
pFile "$entete"
pFile "\hline"
pFile "\\# modules "

NUMS="$1"
echo "NUMS equal : $NUMS"
#NUMS="3"

for numModules in $NUMS
do
    pFile "                  & \multicolumn{2}{|c}{$numModules} "
done

pFile  "\\\\\hline" 


for numModules in $NUMS
do
               pFile "   & time      & states"
done

pFile  "\\\\\hline" 
}


exp()
{
opt="$3"
text="$2"
NUMS="$1"
######### With no bug ########
echo "########## Loop with bug ############"
pFile "${text} no bug"
for numModules in $NUMS
do
    echo "-------> With $numModules modules"
    file_exec_nobug="ok.${numModules}.${text}nobug.pr"
    if [ ! -f  "$file_exec_nobug" ]
    then
	./main.exe -print-ir -print-elab -b promela -o "$file_exec_nobug" main.opt.sc.bc -inline -args $numModules $opt
    fi
    
    trace_no_bug="${numModules}.${text}nobug.verif"
    verif "$file_exec_nobug" > "$trace_no_bug"
    nbstates=`cat "$trace_no_bug" | grep ", stored" | cut -f1 -d "s" | tr -d " "`
    veriftime=`cat ressources | grep veriftime | cut -d: -f2`
    echo "RESULTS with ${text} $numModules and no bug : $nbstates"
    pFile "& $veriftime  &  $nbstates "
done

pFile "\\\\\hline"

echo "########## Loop with bug ############"
pFile "${text} bug"
for numModules in $NUMS
do
    ######### With bug ########
    echo "-------> With $numModules modules"
    file_exec_bug="ok.${numModules}.bug.pr"
    if [ ! -f  "$file_exec_bug" ]
    then
	echo "> ./main.exe -print-ir -print-elab -b promela -o $file_exec_bug main.opt.sc.bc -inline -bug -args $numModules"
	./main.exe -print-ir -print-elab -b promela -o "$file_exec_bug" main.opt.sc.bc -inline -bug -args $numModules $opt
    fi
    
    trace_bug="${numModules}.${text}bug.verif"
    verif "$file_exec_bug" > "$trace_bug"
    nbstates=`cat "$trace_bug" | grep ", stored" | cut -f1 -d "s" | tr -d " "`
    veriftime=`cat ressources | grep veriftime | cut -d: -f2`

    echo "RESULTS with ${text} $numModules and bug : $nbstates"
    echo "Time : $veriftime"
    
    pFile "&  $veriftime  &  $nbstates "
done
}

oneTab()
{
    NUMS="13 15 17 19 21"
    startTab "$NUMS"
    exp "$NUMS" "normal:" ""
    pFile "\\\\\hline"
    exp "$NUMS" "bool:" "-events-as-bool"
    pFile "  \end{tabular}"
}

twoTabs()
{
    NUMS="3 5 7 9 11"
    startTab "$NUMS"
    exp "$NUMS" "normal:" ""
#    pFile "\\\\\hline"
#    exp "$NUMS" "bool:" "-events-as-bool"
    pFile "  \end{tabular}"

    pFile ""
    
    NUMS="13 15 17 19 21 23"
    startTab "$NUMS"
    exp "$NUMS" "normal:" ""
#    pFile "\\\\\hline"
#    exp "$NUMS" "bool:" "-events-as-bool"
    pFile "  \end{tabular}"
}

twoTabs
#oneTab
