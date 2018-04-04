<html>

  <head>
    <title>Garage Door Opener</title>
  </head>

  <body>

    <h1>Open my garage door!</h1>

    <hr>
    <?php 
      if(isset($_POST['relayon']))
      {
        shell_exec("/home/pi/repos/hc/devcpp/dst/armv7lposixdbg/bin/hciset /relayon 0 true");
      }
      if(isset($_POST['relayoff']))
      {
        shell_exec("/home/pi/repos/hc/devcpp/dst/armv7lposixdbg/bin/hciset /relayon 0 false");
      }
      if(isset($_POST['pulserelayhigh']))
      {
        shell_exec("/home/pi/repos/hc/devcpp/dst/armv7lposixdbg/bin/hcicall /pulserelayhigh 0");
      }
    ?>

    <form method="post">
      <input type="text" name="txt" value="<?php echo shell_exec("/home/pi/repos/hc/devcpp/dst/armv7lposixdbg/bin/hciget /relayon 0"); ?>" >
      <input type="submit" name="relayon" value="Turn Relay On">
      <input type="submit" name="relayoff" value="Turn Relay Off" >
      <input type="submit" name="pulserelayhigh" value="Pulse Relay High" >
    </form>
    <hr>

  </body>

</html>
