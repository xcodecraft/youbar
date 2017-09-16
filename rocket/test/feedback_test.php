<?php
require_once('hydra.php');
class FeedbackTc extends PHPUnit_Framework_TestCase
{
    public function __construct()
    {
    }
    public function __destruct()
    {
    }
    
    public function test_getUndoneEvents()
    {
        $assignedEvents      = array(
            'paylbw.qid222222.sdktest' => 
                array (
                    'name' => 'paylbw',
                    'app' => 'sdktest',
                    'key' => 'qid222222',
                    'data' => '{"money":50}',
                ),
                'paylbw.qid222222.sdktest2' => 
                array (
                    'name' => 'paylbw',
                    'app' => 'sdktest2',
                    'key' => 'qid222222',
                    'data' => '{"money":50}',
                ),
        );
        $doneEvents          = array(
            'paylbw.qid222222.sdktest' => 
            array (
                'name' => 'paylbw',
                'app' => 'sdktest',
                'key' => 'qid222222',
                'data' => '{"money":50}',
            ),
            'paylbw3.qid222222.sdktest' => 
            array (
                'name' => 'paylbw3',
                'app' => 'sdktest',
                'key' => 'qid222222',
                'data' => '{"money":50}',
            ),

        );
        $assignedEvents      = $this->filterOutDoneEvents($assignedEvents,$doneEvents);
        $arr                 = array(
            'paylbw.qid222222.sdktest2' => 
                array (
                    'name' => 'paylbw',
                    'app' => 'sdktest2',
                    'key' => 'qid222222',
                    'data' => '{"money":50}',
                ),
        );
        $this->assertEquals($assignedEvents,$arr);
    }
    public function test_getUndoneEvents_largeAmount()
    {
        $this->assignedEvnetRecordFile = dirname(__FILE__).'/data/result.14';
        $this->doneEvnetRecordFile1    = dirname(__FILE__).'/data/feedback.14';
        $this->doneEvnetRecordFile2    = dirname(__FILE__).'/data/feedback.15';
       
        $assignedEventRecord           = new EventRecordSvc($this->assignedEvnetRecordFile);
        $cnt = $assignedEventRecord->getCnt();

        $page    = 2;
        $start   = 0;
        $doneEventsRecord              = new DoneEventTc();
        $undoneEvents = array();
        while($start < $cnt)
        {
            $t  = $assignedEventRecord->gets($start,$page);
            $assignedEvents = $t['data'];
            $row            = $t['total'];
            if(empty($assignedEvents))
            {
                break;
            }
            foreach($assignedEvents as $k=>$assignedEvent)        
            {
                if($undoneEvents[$k])
                {
                    unset($assignedEvents[$k]);//消重
                    continue;
                }
                if($doneEventsRecord->isEventDone($assignedEvent))
                {
                    unset($assignedEvents[$k]);
                }
            }
            if(empty($assignedEvents))
            {
                break;
            }
            $undoneEvents += $this->checkSubs($assignedEvents);
            $start += $row;
        }
        $this->assertEquals($undoneEvents,array());
    }
    public function filterOutDoneEvents($assignedEvents,$doneEvents)
    {
        if(!empty($doneEvents))
        {
            foreach($assignedEvents as $k=>$v)
            {
                if(isset($doneEvents[$k]))
                {
                    unset($assignedEvents[$k]);
                }
            }
        }
        return $assignedEvents;
    }
    //check if sub is still alive
    public function test_checkSubs()
    {
        $serverconf['server']  = DispatchConf::GM_SERVERS;
        $dataFetcher = new GearmanClient();
        $dataFetcher->addServers( $serverconf['server'] );
        $this->dataFetcher = $dataFetcher;
        $assignedEvents      = array(
            'feedback_test.qid222222.feedback_test' => 
            array (
                'name' => 'feedback_test',
                'app' => 'feedback_test',
                'key' => 'qid222222',
                'data' => '{"money":50}',
            ),
            'feedback_test.qid222222.feedback_test2' => 
            array (
                'name' => 'feedback_test',
                'app' => 'feedback_test2',
                'key' => 'qid222222',
                'data' => '{"money":50}',
            ),
        );
        $legalEvent = $this->checkSubs($assignedEvents);
        $arr        = array (
            'feedback_test.qid222222.feedback_test' => 
            array (
                'name' => 'feedback_test',
                'app' => 'feedback_test',
                'key' => 'qid222222',
                'data' => '{"money":50}',
            ),
        );
        $this->assertEquals($legalEvent,$arr);
    }
    private function checkSubs($events)
    {
        $this->addSub();
        if(empty($events))
        {
            return array();
        }
        $this->dataFetcher->setTimeout(1000);

        $pkg['event'] = 'all' ;
        $status = $this->dataFetcher->do('__status', json_encode( $pkg) );
        $this->assertNotNull($status);
        $tmp = json_decode($status,true);
        $allSubs = $tmp['all'];
        foreach($events as $k=>$event)
        {
            $name = $event['name'];
            $app  = $event['app'];
            $subs = $allSubs[$name];
            if(empty($subs))
            {
                unset($events[$k]);
            }
            else if(!in_array($app,$subs))
            {
                unset($events[$k]);
            }
        }
        return $events;
    }

    public function test_redo()
    {
        $logger =  GLogFinder::sysLogger();
        $serverconf['server']  = DispatchConf::GM_SERVERS;
        $serverconf['logger']  = $logger;
        $gmclient = new GearmanClient();
        $gmclient->addServers( $serverconf['server'] );
        $this->agent    = new gm_agent($gmclient,$logger);

        //proc_times = null
        $event      = array(
            'name' => 'feedback_test',
            'app' => 'feedback_test',
            'key' => 'qid222222',
            'proc_times' => '',
            'data' => '{"money":50}',
        );
        $procTimes = $event['proc_times'];
        if(!empty($procTimes)&&is_numeric($procTimes))
            $procTimes++;
        else
            $procTimes = Hydra::FIRST_TIME_REDO;
        Hydra::log_redo($event['name'].".".$event['app'],$event['key'],$event['data'],$procTimes);
  
        $c = $this->agent->clone_me($event['app']);
        $r = $c->send($event['name'],$event);

        //proc_times = 1 
        $event      = array(
            'name' => 'feedback_test',
            'app' => 'feedback_test',
            'key' => 'qid222222',
            'proc_times' => 1,
            'data' => '{"money":50}',
        );
        $procTimes = $event['proc_times'];
        if(!empty($procTimes)&&is_numeric($procTimes))
            $procTimes++;
        else
            $procTimes = Hydra::FIRST_TIME_REDO;
        Hydra::log_redo($event['name'].".".$event['app'],$event['key'],$event['data'],$procTimes);
  
        $c = $this->agent->clone_me($event['app']);
        $r = $c->send($event['name'],$event);
       
    }
    private function addSub()
    {
        GLogConf::pylonAppConf();
        GlogConf::$console = true;
        GLogConf::debugOn();
        $logger =  GLogFinder::sysLogger();
        $conf1   = HydraConf::init("feedback_test",$logger);
        $h1      = new HydraWorker($conf1);
        $h1->subscribe("feedback_test",array($this,"event_test_proc"));
        $h1->wait_event(array($this,stop_it));
    }
    public function stop_it()
    {
        $data["money"] = 50 ; 
        Hydra::trigger("feedback_test","qid222222",json_encode($data));
        static $i=0; 
        $i++;
        if( $i==1 )
        {
            return true;
        }
        return false;
    }
    private function event_test_proc($job)
    {
    }
}
class EventRecordTc extends PHPUnit_Framework_TestCase
{
    public function __construct()
    {
        $this->assignedEvnetRecordFile = dirname(__FILE__).'/data/result.14';
        $this->doneEvnetRecordFile1    = dirname(__FILE__).'/data/feedback.14';
        $this->doneEvnetRecordFile2    = dirname(__FILE__).'/data/feedback.15';
    }
    public function test_getCnt()
    {
        $recordSvc = new EventRecordSvc($this->assignedEvnetRecordFile);
        $assignedEventsCnt = $recordSvc->getCnt();

        $recordSvc = new EventRecordSvc($this->doneEvnetRecordFile1);
        $doneCnt1  = $recordSvc->getCnt();

        $recordSvc = new EventRecordSvc($this->doneEvnetRecordFile2);
        $doneCnt2  = $recordSvc->getCnt();
        $arr       = array($assignedEventsCnt,$doneCnt1,$doneCnt2);
        $this->assertEquals($arr,array(16,6,9));
    }
    public function test_gets()
    {
        $recordSvc = new EventRecordSvc($this->assignedEvnetRecordFile);
        $assignedEvents = $recordSvc->gets();
        $arr       =  array (
            'total' => 16,
            'data' => 
            array (
                'paylbw.qid222222.sdktest' => 
                array (
                    'name' => 'paylbw',
                    'app' => 'sdktest',
                    'key' => 'qid222222',
                    'proc_times' => '',
                    'data' => '{"money":50}',
                ),
                'paylbw.qid222222.sdktest2' => 
                array (
                    'name' => 'paylbw',
                    'app' => 'sdktest2',
                    'key' => 'qid222222',
                    'proc_times' => '',
                    'data' => '{"money":50}',
                ),
            ),
        );
        $this->assertEquals($arr,$assignedEvents);
    }
    public function test_gets_pagely()
    {
        $recordSvc = new EventRecordSvc($this->assignedEvnetRecordFile);
        $start     = 0;
        $limit     = 1;
        $assignedEvents = $recordSvc->gets($start,$limit);
        $arr       = array (
            'total' => 1,
            'data' => 
            array (
                'paylbw.qid222222.sdktest' => 
                array (
                    'name' => 'paylbw',
                    'app' => 'sdktest',
                    'key' => 'qid222222',
                    'proc_times' => '',
                    'data' => '{"money":50}',
                ),
            ),
        );
        $this->assertEquals($assignedEvents,$arr);

        $arr = array();
        $assignedEvents = array();
        $start     = 1;
        $limit     = 1;
        $assignedEvents = $recordSvc->gets($start,$limit);
        $arr       = array (
            'total' => 1,
            'data' => 
            array (
                'paylbw.qid222222.sdktest2' => 
                array (
                    'name' => 'paylbw',
                    'app' => 'sdktest2',
                    'key' => 'qid222222',
                    'proc_times' => '',
                    'data' => '{"money":50}',
                ),
            ),
        );
        $this->assertEquals($assignedEvents,$arr);

        $start     = 2;
        $limit     = 2;
        $assignedEvents = $recordSvc->gets($start,$limit);
        $arr       = array (
            'total' => 2,
            'data' => 
            array (
                'paylbw.qid222222.sdktest' => 
                array (
                    'name' => 'paylbw',
                    'app' => 'sdktest',
                    'key' => 'qid222222',
                    'proc_times' => '',
                    'data' => '{"money":50}',
                ),
                'paylbw.qid222222.sdktest2' => 
                array (
                    'name' => 'paylbw',
                    'app' => 'sdktest2',
                    'key' => 'qid222222',
                    'proc_times' => '',
                    'data' => '{"money":50}',
                ),
            ),
        );
        $this->assertEquals($assignedEvents,$arr);
    }
}
class DoneEventTc extends PHPUnit_Framework_TestCase
{
    public function __construct()
    {
        $this->recordFile  = dirname(__FILE__).'/data/feedback.14';
        $this->recordFile2 = dirname(__FILE__).'/data/feedback.15';
    }
    public function test_isEventDone()
    {
        $assignEvent = array(
            'name' => 'paylbw',
            'app' => 'sdktest',
            'key' => 'qid222222',
            'data' => '{"money":50}',
        );
        $assignEvent1 = array(
            'name' => 'paylbw',
            'app' => 'sdktest',
            'key' => 'qid222222',
            'data' => '{"money":50}',
        );
        $unassignedEvent = array(
            'name' => 'paylbw_illegal',
            'app' => 'sdktest',
            'key' => 'qid222222',
            'data' => '{"money":50}',
        );
        $res = array();
        $exist[] = $this->isEventDone($assignEvent);
        $exist[] = $this->isEventDone($assignEvent1);
        $exist[] = $this->isEventDone($unassignedEvent);
        $this->assertEquals($exist,array(true,true,false));
    }
    public function isEventDone($assignEvent)
    {
        $res = array();
        $assignEventKey = $assignEvent['name'].'.'.$assignEvent['key'].'.'.$assignEvent['app'];
        if($this->isRecordInFile($assignEventKey,$this->recordFile)||$this->isRecordInFile($assignEventKey,$this->recordFile2))
        {
            return true;
        }
        return false;

    }
    private function isRecordInFile($str,$file)
    {
        if(empty($file)||empty($str))
        {
            return false;
        }
        $handle = @fopen($file,"r");
        if(!$handle)
        {
            return false;
        }
        while (!feof($handle))
        {
            $line = trim(fgets($handle));
            $event = $this->getEvent($line);
            $key = $event['name'].'.'.$event['key'].'.'.$event['app'];
            if($key == $str)
            {
                fclose($handle);
                return true;
            }
        }
        fclose($handle);
        return false;
    } 
    private function getEvent($eventRecord)
    {
        $arr       = explode(' ',$eventRecord);
        $eventStr  = $arr[3];
        $eventArr  = explode('#',$eventStr);
        $eventArr[]= $arr[4];
        $event     = $this->fmtEvent($eventArr);
        return $event;
    }
    private function fmtEvent($eventArr)
    {
        $event = array(); 
        $tmp   = explode('.',$eventArr[1]);
        $event['name' ] = $tmp[0];
        $event['app' ]  = $tmp[1];
        $event['key' ]  = $eventArr[2];
        $event['proc_times' ]  = $eventArr[3];
        $event['data']  = $eventArr[4];
        return $event;
    }
}
