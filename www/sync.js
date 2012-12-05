//用于消息同步
//不停从c程序中获取消息
//所有从c程序发送到网页的消息都经过这里

sync_myuserid = 0;

function checkTime(i)
{
if (i<10) 
  {i="0" + i}
  return i
}
function update(){
	var today=new Date()
	var h=today.getHours()
	var m=today.getMinutes()
	s=today.getSeconds()
	m=checkTime(m)
	s=checkTime(s)
	$('#update').html(h+":"+m+":"+s);
}
function sync(){
	do_cmd("g "+sync_myuserid, function(json,status){
		update();
		if(json.type == "new"){
				sync_myuserid = json.id;
				sync();
		}else if(json.type == "msg"){
				if(json.msg.type == "machine"){
					$('#machine-'+json.msg.id+'-ip').html(json.msg.ip+':'+json.msg.port);
					if(json.msg.status == 0){
						$('#machine-'+json.msg.id+'-status').html('inactive');
						$('#machine-'+json.msg.id+'-status').css("color", "black");
					}else {
						$('#machine-'+json.msg.id+'-status').html('active');
						$('#machine-'+json.msg.id+'-status').css("color", "red");
					}
					
					for(var i=0; i < 16 ; i++){
						if(json.msg.disks[i].status == 0){
							$('#disk-'+json.msg.id+'-'+i).html('inactive');
							$('#disk-'+json.msg.id+'-'+i).css("color", "black");
						}else {
							$('#disk-'+json.msg.id+'-'+i).html(json.msg.disks[i].capacity+' GB');
							$('#disk-'+json.msg.id+'-'+i).css("color", "red");
						}
					}
					
				}else if(json.msg.type == "textresult"){
					$('#text-results-content').append('<li>'+json.msg.text+'</li>');
				}else if(json.msg.type == "result"){
					if(json.msg.test == "iometer"){
						results["iops"][json.msg.machine][json.msg.disk][0].shift();
						results["iops"][json.msg.machine][json.msg.disk][0].push(json.msg.iops);
					}else if(json.msg.test == "bit"){
						results["readiops"][json.msg.machine][json.msg.disk][0].shift();
						results["readiops"][json.msg.machine][json.msg.disk][0].push(json.msg.readiops);
						results["writeiops"][json.msg.machine][json.msg.disk][0].shift();
						results["writeiops"][json.msg.machine][json.msg.disk][0].push(json.msg.writeiops);
						results["readspeed"][json.msg.machine][json.msg.disk][0].shift();
						results["readspeed"][json.msg.machine][json.msg.disk][0].push(json.msg.readspeed);
						results["writespeed"][json.msg.machine][json.msg.disk][0].shift();
						results["writespeed"][json.msg.machine][json.msg.disk][0].push(json.msg.writespeed);
					}
				}else if(json.msg.type == "chat"){
					$('#chat-messages').prepend('<li>'+json.msg.msg+'</li>');
				}else{
					prompt(json.msg);
				}
				sync();
		}else if(json.type == "none"){
				setTimeout("sync()", interval);
		}
    });
}


$(document).ready(function(){
	sync();
});
