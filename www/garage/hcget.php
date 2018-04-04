<?Php
  $data = array('pid' => $_GET['pid'], 'val' => trim(shell_exec('./hcget 127.0.0.1 1500 ' . $_GET['pid'])));
  echo json_encode($data);
?>
