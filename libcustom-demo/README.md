# libcustom-demo

This is the custom demo library to be loaded within the pollux-core custom library.

The way it works:

1. Load the configuration from `/etc/pollux/custom-demo.json` if exists
2. According to the metrics configured at the `custom-demo.json` it will be sent on changes:
3. The tracking is configured to enqueue data of the position on the following scenarios:

   - **On movement and position fixed**: Send every 15 seconds if the distance between the previous position and the next is > 50 m.
   - **On STOP and position fixed**: Send the position every 300 seconds (5 minute)
   - **On DIO0/MOVE change**: Send the event
   - **On no position**: Nothing is sent but ignition change

4. The data are enqueued into the `redis.queue` parameter


