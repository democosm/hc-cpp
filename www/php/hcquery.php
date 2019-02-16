<?php

//Get query string
$querystr = $_GET["cmd"];

//Send to query server and get resulting output
$result = shell_exec("./hcquery 127.0.0.1 5555 $querystr");

//Echo result
echo $result;

?>
