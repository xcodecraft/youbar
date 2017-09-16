adirname() { odir=`pwd`; cd `dirname $1`; pwd; cd "${odir}"; }
MYDIR=`adirname "$0"`
$MYDIR/../sdk/setup.sh dev
RG=/home/q/tools/pylon_rigger/rigger
#sudo $RG  conf,restart -e dev,debug -s event_log,gearmand
sudo $RG  conf,restart -e dev,debug -s event_log,gearmand,center,test,monitor,econf
#sudo $RG  conf,restart -e dev,debug -s event_log,gearmand,center,test,monitor
