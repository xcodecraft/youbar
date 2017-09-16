<?php
require_once('hydra.php');
require_once('/home/q/php/svc_utls/logger.php');
class rece_tc extends PHPUnit_Framework_TestCase
{
    public function test_trigger()
    {
        $data["money"] = 500 ;                   
        Hydra::trigger("event","qid:1001",json_encode($data));
    }
    public function c1test_receiver()
    {
        return ;
        $logger =  GLogFinder::sysLogger();
        $gmclient = new GearmanClient();
        $gmclient->addServers( "127.0.0.1:4730" );

        $gmwork   = new GearmanWorker();
        $gmwork->addServers( "127.0.0.1:4730" );

        $reciver = new reciver($gmwork,$logger);
        $agent   = new gm_agent($gmclient,$logger);
        $dispatch = new dispatcher( $datasvc, $agent, $logger);

        $reciver->register('subscribe',  array('dispatch','on_subscribe') );
        $reciver->register('unsubscribe', array('dispatch','on_unsubscribe') );
        $reciver->register('event', array('dispatch','on_recive'));

        $reciver->serving_recv();
        
    }
}
