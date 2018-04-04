<?Php
  $data = array('pid' => $_GET['paramname'], 'val' => trim(shell_exec('./hcget 127.0.0.1 1500 ' . $_GET['paramname'])));
  echo json_encode($data);
?>
