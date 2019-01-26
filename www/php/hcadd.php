<?php

//Decode query string as JSON
$params = json_decode($_GET["params"]);

//Check for error decoding query string
if($params == null)
{
  echo("Syntax: hcadd.php?params=JSON array of parameter names and values<br>");
  echo("Example: hcadd.php?params=[[\"/foo\",1],[\"/bar\",\"hello\"]]");
  http_response_code(400);
  exit;
}

//Do this for each parameter
foreach($params as $param)
{
  //Call HC command line app and trim result
  $result = trim(shell_exec("./hcadd 127.0.0.1 1500 $param[0] $param[1] server.nld"));

  //Update response
  $response[] = array("$param[0]", "$result");
}

//Format as JSON
echo json_encode($response);

?>