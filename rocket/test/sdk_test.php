<?php
//require_once('/home/q/php/hydra_sdk/hydra.php');
require_once(dirname(__FILE__).'/../sdk/hydra.php');
class SDKTest  extends PHPUnit_Framework_TestCase
{
    public function test_A()
    {
        $data["money"] = 50 ; 
        Hydra::trigger("paylbw","qid222222",json_encode($data));
    }

    public function testHydraWorker()
    {
        GLogConf::pylonAppConf();
        GlogConf::$console = true;
        GLogConf::debugOn();
        $logger =  GLogFinder::sysLogger();
        $conf1   = HydraConf::init("sdktest",$logger);
        $h1      = new HydraWorker($conf1);
        $h1->subscribe("paylbw",array($this,"event_test_proc"));
        $h1->wait_event(array($this,stop_it));
    }
  
  
    public function stop_it()
    {
        $data["money"] = 50 ; 
        Hydra::trigger("paylbw","qid222222",json_encode($data));
        static $i=0; 
        $i++;
        echo "stop $i\n";
        if( $i==2 )
        {
            return true;
        }
        return false;
    }

    public function event_test_proc($job)
    {
//        var_dump(json_decode($job['data'],true));
    }

}
