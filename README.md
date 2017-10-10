OSGi Manger is a utility to auto manage your OSGi bundles using a simple configuration file.

OSGi Manager can manages your Apache Camel's / JBoss ESB's at a bundle level as opposed to http PING requests  which can be very inefficient on the long run. 

With OSGi manager you don't have to restart the entire JBoss / Apache camel instance because of a defective bundle. 

Also, OSGi Manager relieve Apache Camel / JBoss systems from perpetual flaws of PING monitoring system.  Since the PING requests sent while monitoring is treated has a normal transaction requests, this can lead to resource exhaustion and resource choke, and as result, the affected instance can become unstable and eventually restarts ( a denial of service ).

Also the OSGi Manager is automated, you dont have to manually login to JBoss ESB GoGo to manage your bundles.