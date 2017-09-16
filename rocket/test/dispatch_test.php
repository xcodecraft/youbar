<?php
class dispatch
{
    private $logger;
    private $store;
    private $agent;

    public function __construct($datasvc,$agent,$logger)
    {
        $this->store   = $datasvc;
        $this->agent   = $agent;
        $this->logger  = $logger ;
    }
    public function on_subscribe( $job )
    {
        $subscriber = $job['app'];
        $event      = $job['event'];

        $this->store->subscribe($event, $subscriber); 

        $pkg['errno_no'] = 0;
        return json_encode($pkg);
        
    }
    public function on_unsubscirbe($job )
    {
        $event = $workload['event'];
        $app   = $workload['app'];

        $this->store->unsubscribe( $event,$app);

        $pkg['errno_no'] = 0;
        return json_encode($pkg);
    }
    public function on_recive($event)
    {
        $name = $event['name'];
        $subscribed = $this->store->list_subscribes($name);
        foreach ($subscribed as $i)
        {
            $c = $this->agent->clone_me($i);
            $c->send($name,$event);
        }
    }

}
class gmwork
{
    public function serving_recv()
    {
    } 
}
class data_sub
{
    public function __call($name,$params)
    {
    }
    public function list_subscribes()
    {
        return array("a","b","c");
    }
}

class agent 
{ 
    private $gc;
    private $id;

    public function clone_me($id)
    {
        $other = clone $this;
        $other->id = $id;
        return $other;
    }

    public function send( $name, $event )
    {
        return array($name,$event);
    }
}
class dispatch_tc extends PHPUnit_Framework_TestCase
{

//    public function test_agent()
//    {
//        GLogConf::$app="dispatch";
//        GLogConf::debugOn();      
//        $datasvc = new data_sub();
//        $gmwork   = new GearmanWorker();
//        $logger = GLogFinder::sysLogger();

//        $agent_mock = $this->getMock('agent',array('clone_me','send')); 
//        $agent_mock->expects( $this->exactly(3))->method('clone_me')->will($this->returnSelf());;
//        $agent_mock->expects( $this->exactly(3))->method('send');

//        return $agent_mock;
//    }

//    /**
//     ** @depends test_agent 
//     **/
    public function test_dispatch()
    {
        GLogConf::$app="dispatch";
        GLogConf::debugOn();       
        $datasvc = new data_sub();
        $gmwork   = new GearmanWorker();
        $logger = GLogFinder::sysLogger();

        $event['name'] = 'pay';
        $event['detail'] ='hello';

        $result =  array('pay',$event );

        $agent_mock = $this->getMock('agent',array('clone_me','send')); 
        $agent_mock->expects( $this->any())->method('clone_me')->will($this->returnSelf());;
        $agent_mock->expects( $this->any())->method('send')->will( $this->returnValue($result));
    
        $this->assertEquals($result , $agent_mock->send('pay',$event));

        $dispatch = new dispatcher( $datasvc, $agent_mock, $logger);
        $dispatch->on_recive( $event);
    }
    
//    public function test_reciver()
//    {
//        GLogConf::$app="dispatch";
//        GLogConf::debugOn();       
//        $logger = GLogFinder::sysLogger();

//        $datasvc = new data_svc();
//        $agent   = new gm_agent();
//        $gmwork  = new GearmanWorker();

//        $dispatch = new dispatcher( $datasvc,$agent,$logger);
//        $reciver  = new reciver( $gmwork, $logger);

//        $reciver->register('subscribe',  array('dispatch','on_subscribe') );
//        $reciver->register('unsubscribe', array('dispatch','on_unsubscribe') );
//        $reciver->register('event', array('dispatch','on_recive') );

//        $reciver->serving_recv();

//    }
}
