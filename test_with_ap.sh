
mode=${1}

SLEEPTIME=5
read up rest </proc/uptime; start="${up%.*}${up#*.}"

if [ ${mode} == "tx" ]; then
	uci set wireless.ap.disabled=1 && uci commit && wifi &
fi
sleep ${SLEEPTIME}

read up rest </proc/uptime; end="${up%.*}${up#*.}"
runtime=$((10*(${end}-${start})))
echo "AP testmode waittime: ${runtime}"

./test.sh testids.csv ${mode}

if [ ${mode} == "tx" ]; then
        uci set wireless.ap.disabled=0 && uci commit && wifi &
fi

