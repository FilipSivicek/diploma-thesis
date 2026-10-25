#sizes=("5" "6" "9" "10" "11" "12" "13" "14")
#sizes=("12" "13")
sizes=("11")

for i in ${!sizes[@]}; do 
    ./two_hamiltons reducable_graphs/${sizes[$i]}.txt > reduced_graphs/${sizes[$i]}.txt
done