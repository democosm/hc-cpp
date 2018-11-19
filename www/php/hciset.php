<?php

//Decode query string as JSON
$params = json_decode($_GET["params"]);

//Check for error decoding query string
if($params == null)
{
  echo("Syntax: hciset.php?params=JSON array of parameter names, eids and values<br>");
  echo("Example: hciset.php?params=[[\"/foo\",1,3],[\"/bar\",4,\"hello\"]]");
  http_response_code(400);
  exit;
}

//Do this for each parameter
foreach($params as $param)
{
  //Call HC command line app and trim result
  $result = trim(shell_exec("./hciset 127.0.0.1 1500 $param[0] $param[1] $param[2] server.nld"));

  //Update response
  $response[] = array("$param[0]", "$param[1]", "$result");
}

//Format as JSON
echo json_encode($response);

?>
