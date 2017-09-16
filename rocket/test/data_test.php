<?php
class data_tc extends PHPUnit_Framework_TestCase
{
    public function testData()
    {
        GLogConf::$app="hydra_ut";
        GLogConf::debugOn();                                             
        $bdbfile  ="/tmp/testdatasvc.bdb";
        system("rm -rf $bdbfile");
        $logger =  GLogFinder::sysLogger();
        $svc = new data_svc($bdbfile,$logger);
        $svc->subscribe("test","a");
        $svc->subscribe("test","b");
        $svc->subscribe("test","c");

        $keys = $svc->list_all_events();
        $this->assertEquals( $keys, array('test##subscribed') );
        $subs = $svc->list_all_subscribes( $keys );
        $this->assertEquals( $subs, array('test'=>array('a','b','c')));

        $subs = $svc->list_subscribes("test");
        $expect = array("a","b","c");
        $this->assertEquals($subs, $expect);

        $svc->unsubscribe("test","c");
        $expect = array("a","b");
        $subs = $svc->list_subscribes("test");
        $this->assertEquals($subs, $expect);


        $subs = $svc->list_subscribes("test_1");
        $this->assertEquals($subs, array());

        $subs = $svc->subscribe("key",'a');
        $subs = $svc->subscribe("key",'b');
        $subs = $svc->subscribe("key",'c');

        $subs = $svc->subscribe("key1",'a');
        $subs = $svc->subscribe("key1",'b');
        $subs = $svc->subscribe("key1",'c');

        $subs = $svc->subscribe("__key",'a');
        $subs = $svc->subscribe("__key",'a');//
        $subs = $svc->subscribe("__key",'b');

        $event = $svc->list_all_events();
        $this->assertEquals($event,array('__key##subscribed','key##subscribed','key1##subscribed','test##subscribed'));

        $eventsubs = $svc->list_all_subscribes( $event );
        $this->assertEquals( $eventsubs, array('__key'=>array('a','b'),'test'=>array('a','b'),'key'=>array('a','b','c'),'key1'=>array('a','b','c')));

        $svc->unsubscribe("key","a");
        $svc->unsubscribe("key","b");

        $event = $svc->list_all_events();
        $this->assertEquals($event,array('__key##subscribed','key##subscribed','key1##subscribed','test##subscribed'));

        $eventsubs = $svc->list_all_subscribes( $event );
        $this->assertEquals( $eventsubs, array('__key'=>array('a','b'),'test'=>array('a','b'),'key'=>array('c'),'key1'=>array('a','b','c')));
    }

   public function t2est__data()
    {
        GLogConf::$app="hydra_ut";
        GLogConf::debugOn();                                             
        $bdbfile  ="/tmp/testdatasvc_1.bdb";
        system("rm -rf $bdbfile");
        $logger =  GLogFinder::sysLogger();
        $svc = new data_svc($bdbfile,$logger);
        $svc->subscribe("__test","a");
        $svc->subscribe("__test","b");
        $svc->subscribe("__test","c");

        $subs = $svc->list_subscribes("__test");
        $expect = array("a","b","c");
        $this->assertEquals($subs, $expect);

        $keys = $svc->list_all_events();
        $this->assertEquals( $keys, array() );


        $subs = $svc->subscribe("__key",'a');
        $subs = $svc->subscribe("__key",'b');
        $subs = $svc->subscribe("__key",'c');

        $subs = $svc->subscribe("key1",'a');
        $subs = $svc->subscribe("key1",'b');
        $subs = $svc->subscribe("key1",'c');

        $subs = $svc->subscribe("_key",'a');
        $subs = $svc->subscribe("_key",'a');//
        $subs = $svc->subscribe("_key",'b');

        $event = $svc->list_all_events();
        $this->assertEquals($event,array('_key##subscribed','key1##subscribed'));

        $eventsubs = $svc->list_all_subscribes( $event );
        $this->assertEquals( $eventsubs, array('_key'=>array('a','b'),'key1'=>array('a','b','c')));

    }

}
