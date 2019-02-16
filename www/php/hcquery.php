<?php

//Call HC command line app, trim output and echo
echo trim(shell_exec("./hcquery 127.0.0.1 5555 $_GET["cmd"]"));

?>
