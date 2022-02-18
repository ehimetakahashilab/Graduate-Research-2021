// HTMLが読み込まれた後の処理
$(function(){
	"use strict";
	
	// "パケット情報テーブル"の高さ設定
	$(document).ready(function(){
		var height = $(window).height() - 190;
		$("#infoTableBody").height(height);
	});
	$(window).on("resize", function(){
		var height = $(window).height() - 190;
		$("#infoTableBody").height(height);
	});
	
	// "パケット情報"ボタンクリック時
	$("#infoTableButton").click(function(){
		var $area = $("#infoTableArea");
		
		if( $area.is(":visible") ){
			$area.fadeOut("slow");
		}else{
			$("#menuArea").fadeOut("fast");
			$area.fadeIn("slow");
		}
	});
	
	// "可視化メニュー"ボタンクリック時
	$("#menuButton").click(function(){
		var $area = $("#menuArea");
		
		if( $area.is(":visible") ){
			$area.fadeOut("slow");
		}else{
			$("#infoTableArea").fadeOut("fast");
			$area.fadeIn("slow");
		}
	});
	
	
});

(function() {
"use strict";

// --------------------------------------------- 変数宣言
var renderer;
var scene;
var camera;
var controls;
var baseTime;

var radius = 20;

var ip = [];

var flow = [];

var flag = [];

var socket = null;

var marker = 0;
var flagmarker = 0;
var MAX_FLOW = 300;  // 表示可能なパケットフローの上限
var MAX_COUNTRY = 2;


// --------------------------------------------- window イベント
// 読み込み時のイベント
$(function(){
	init();
});

// リサイズ時のイベント
$(window).resize(function() {
	renderer.setSize(window.innerWidth, window.innerHeight);
	camera.aspect = window.innerWidth / window.innerHeight;
	camera.updateProjectionMatrix();
});


// --------------------------------------------- 読み込み時の処理
function init() {
	initThree();
}


// 可視化モデルの準備
function initModel() {
	// シーン作成
	scene = new THREE.Scene();
	
	// ライト作成
	var ambient = new THREE.AmbientLight(0xffffff);
	scene.add(ambient);

	// テクスチャ読み込み
	var textureLoader = new THREE.TextureLoader();
	var texture = textureLoader.load('earthled.png');
	// 地球儀(裏面)
	var earthGeometry = new THREE.SphereGeometry(radius, 36, 36);
	var earthBackMaterial = new THREE.MeshStandardMaterial({
		//color: 0xffffff,
		//color:0x008080,
		color: 0x696969,
		roughness: 1.0,
		metalness: 0.2,
		opacity:0.5,
		//wireframe: true,
		transparent: true,
		map: texture,
		side: THREE.BackSide });
	scene.add( new THREE.Mesh(earthGeometry, earthBackMaterial) );
	var earthwire = new THREE.SphereGeometry(radius, 36, 18);
	var earthwireMaterial = new THREE.MeshStandardMaterial({
		//color: 0x5f9ea0,
		color: 0xff0000,
		roughness: 1.0,
		matalness: 0.3,
		wireframe: true,
		transparent: true,
		opacity:0.4 });
	scene.add( new THREE.Mesh(earthwire, earthwireMaterial) );
	// 地球儀(表面)
	var earthFrontMaterial = new THREE.MeshStandardMaterial({
		color: 0xffffff,
		//color:0x6699FF,
		//color:0x000080,
		roughness: 1.0,
		metalness: 0.2,
		opacity:0.5,
		//wireframe: true,
		transparent: true,
		map: texture,
		side: THREE.FrontSide });
	scene.add( new THREE.Mesh(earthGeometry, earthFrontMaterial) );
	// 背景が地球儀に透けてしまわないための措置
	var geometry = new THREE.SphereGeometry(radius+0.1, 36, 18);
	var material = new THREE.MeshStandardMaterial({
		color: 0x000000,
		side: THREE.BackSide });
	scene.add( new THREE.Mesh(geometry, material) );

	// 宇宙
	
	var spaceGeometry = new THREE.Geometry();
	for (var i = 0; i < 2000; i++) {
		var phi = Math.random() * Math.PI * 2;
		var theta = Math.random() * Math.PI * 2;
		spaceGeometry.vertices.push(new THREE.Vector3(
			1000 * Math.cos(phi) * Math.cos(theta),
			1000 * Math.sin(phi),
			1000 * Math.cos(phi) * Math.sin(theta)));
	}
	var spaceMaterial = new THREE.PointsMaterial({size: 2, color: 0x5f9ea0});
	var space = new THREE.Points(spaceGeometry, spaceMaterial);
	scene.add(space);
	
	// 自分のPCに見立てた立方体
	var geometry = new THREE.CubeGeometry(1, 1, 1);
	var material = new THREE.MeshStandardMaterial();
	var mesh = new THREE.Mesh(geometry, material);
	scene.add(mesh);
		
	return scene;
}

function initThree() {
	// canvas領域の取得
	var canvasFrame = document.getElementById("canvasArea");

	// レンダラ初期化
	renderer = new THREE.WebGLRenderer({ antialias: true});
	renderer.setSize(window.innerWidth, window.innerHeight);
	renderer.setClearColor(0x000000, 1);
	canvasFrame.appendChild(renderer.domElement);

	// カメラ作成
	camera = new THREE.PerspectiveCamera(60, window.innerWidth / window.innerHeight);
	camera.position.set(0, 0, 50);
	// カメラコントロール作成
	controls = new THREE.OrbitControls(camera);
	controls.autoRotate = true;

	scene = initModel();
	
	var loader = new THREE.JSONLoader();
	loader.load('monitor.json', function (geometry, materials)
	{
  	  var geo = geometry;
  	  var mat = new THREE.MeshFaceMaterial(materials);
  	  var mesh = new THREE.Mesh(geo, mat);
  	  scene.add(mesh);
	});
	
	initFlow();

	baseTime = +new Date;
	render();
	
	startNetwork();
}




// --------------------------------------------- レンダリング
function render() {
	requestAnimationFrame(render);
	// カメラコントロールの状態を更新
	controls.update();
	
	updateFlow();
	
	renderer.render(scene, camera);
};


// パケットフローオブジェクトをあらかじめ準備しておく
function initFlow() {
	for(var i=0; i < MAX_FLOW; i++) {
		var packet = {};
		
		// パケットの経路に見立てた線分
		var lineGeometry = new THREE.Geometry();
		lineGeometry.vertices.push(new THREE.Vector3(0, 0, 0));
		lineGeometry.vertices.push(new THREE.Vector3(0, 0, 0));
		var lineMaterial = new THREE.LineBasicMaterial({color: 0xffffff, linewidth: 1, transparent: false, opacity: 0.1});
		var line = new THREE.Line(lineGeometry, lineMaterial);
		
		// パケットに見立てた球体
		var sphereGeometry = new THREE.SphereGeometry(0.5);
		var sphereMaterial = new THREE.MeshStandardMaterial({color: 0xffffff});
		var sphere = new THREE.Mesh(sphereGeometry, sphereMaterial);
		
		line.visible = false;
		sphere.visible = false;
		
		scene.add(line);
		scene.add(sphere);
		
		// オブジェクトに格納
		packet.line = line;
		packet.lineG = lineGeometry;
		packet.lineM = lineMaterial;
		packet.sphere = sphere;
		packet.sphereG = sphereGeometry;
		packet.sphereM = sphereMaterial;
		packet.progress = 1.0;
		packet.available = true;
		
		flow.push(packet);
	}
}


// パケットフローオブジェクトを追加する
function setFlow(pkt) {

	var packet = flow[marker];
	marker = (marker + 1) % MAX_FLOW;

	// 仰角
	var phi = pkt.latlng[0] * (Math.PI / 180);
	// 方位角
	var theta = pkt.latlng[1] * (Math.PI / 180) * -1;

	// ホワイトリストに存在するパケットなら色を緑
	/*if(pkt.white == 0) {
		var packetColor = 0x00ff00;
		packet.line.visible = false;
		packet.sphere.visible = false;
	}
	else {*/
	var packetColor = 0xff0000;
	packet.line.visible = true;
	packet.sphere.visible = true;
	setmark(radius * Math.cos(phi) * Math.cos(theta),radius * Math.sin(phi),radius * Math.cos(phi) * Math.sin(theta),phi);
		//setmark(radius * Math.cos(phi) * Math.cos(theta),radius * Math.sin(phi),radius * Math.cos(phi) * Math.sin(theta))
	//}

	// パケットの経路の設定
	var line = packet.line;
	var lineGeometry = packet.lineG;
	lineGeometry.verticesNeedUpdate = true;
	lineGeometry.vertices[1].x = radius * Math.cos(phi) * Math.cos(theta);
	lineGeometry.vertices[1].y = radius * Math.sin(phi);
	lineGeometry.vertices[1].z = radius * Math.cos(phi) * Math.sin(theta);
	var lineMaterial = packet.lineM;
	lineMaterial.color.setHex(packetColor);
	
	// パケットに見立てた球体の設定
	var sphereMaterial = packet.sphereM;
	sphereMaterial.color.setHex(packetColor);
	
	// シーンに追加
	/*
	if(packet.available == true) {
		packet.line.visible = true;
		packet.sphere.visible = true;
	}
	*/
	
	// パケットフローオブジェクトに情報を追加する
	packet.phi = phi;
	packet.theta = theta;
	packet.progress = 0.0;
	packet.available = false;
	packet.direction = pkt.direction;
}


// パケットフローオブジェクトの状態を更新する
function updateFlow() {
	for(var i=0; i < MAX_FLOW; i++) {
		var packet = flow[i];
		
		// パケットフローが空なら何もしない
		if(packet.available == true) {
			continue;
		}	
		
		if(packet.progress < 1.0) {
			var phi = packet.phi;
			var theta = packet.theta;
			var direction = packet.direction;
			var progress = packet.progress;
	
			var sphere = packet.sphere;
			if(direction == 0) {
				sphere.position.x = radius * Math.cos(phi) * Math.cos(theta) * progress;
				sphere.position.y = radius * Math.sin(phi) * progress;
				sphere.position.z = radius * Math.cos(phi) * Math.sin(theta) * progress;
			} else {
				sphere.position.x = radius * Math.cos(phi) * Math.cos(theta) * (1.0 - progress);
				sphere.position.y = radius * Math.sin(phi) * (1.0 - progress);
				sphere.position.z = radius * Math.cos(phi) * Math.sin(theta) * (1.0 - progress);
			}
			
			packet.progress += 0.01;
		} else {
			packet.sphere.visible = false;
			packet.line.visible = true;
			packet.available = false;
			/*
			if(packet.white == 1) {
				setmark(sphere.position.x,sphere.position.y,sphere.position.z,theta);
			}
			else {
			}
			*/
		}
	}
}


function setmark(x,y,z,phi) {
	
	var textureLoader = new THREE.TextureLoader();
	var texture = textureLoader.load('fire.jpg');
	//for(var i=0; i < MAX_COUNTRY; i++) {
	//不正通信先をハイライトするオブジェクト
	/*
	var geometry = new THREE.CylinderGeometry(1, 1, 2, 25, 25, true);
	var material = new THREE.MeshBasicMaterial({
		color: 0xffffff,
		map: texture,
		transparent: true
		//blending: THREE.AdditiveBlending,
		//side: THREE.FrontSide
	});
	*/

	var geometry = new THREE.SphereGeometry( 0.5, 16, 16);
	//var geometry = new THREE.TorusKnotGeometry(0.2, 0.5, 30, 8, 2, 5, 4);
	var material = new THREE.MeshBasicMaterial({color: 0xffB22222,transparent: true});
	var circle = new THREE.Mesh(geometry, material);

	circle.visible = true;

	circle.position.x = x*1.05;
	circle.position.y = y*1.05;
	circle.position.z = z*1.05;
	//circle.rotation.z = phi;

	//scene.add(circle);

	//オブジェクトに格納
	flag[flagmarker] = circle;

	scene.add(flag[flagmarker]);

	flagmarker = (flagmarker + 1) % MAX_COUNTRY;

	setInterval(removeObject,1000);
	
//}
}

function removeObject(){

	for(var i=0; i<MAX_COUNTRY;i++){
		var obj = flag[i];
		scene.remove(obj);
		return;
	}
}


/*
function setmark(x,y,z) {		
	//if( flagmarker < MAX_COUNTRY ){
	var textureLoader = new THREE.TextureLoader();
	var texture = textureLoader.load('fire.jpg');
	const geometry = new THREE.CylinderGeometry(0.5, 0.5, 0.5, 25, 25, true);
	const geometry = new THREE.TorusGeometry(0.2,0.2,2,100);
	const material = new THREE.MeshBasicMaterial({
		color: 0xffffff,
		map: texture,
		transparent: true
		//blending: THREE.AdditiveBlending,
		//side: THREE.FrontSide
	});
	var mark = new THREE.Mesh(geometry,material);	
	

	var mark = flag[flagmarker];
	flagmarker = (flagmarker + 1) % MAX_COUNTRY;
	
	//通信場所に表示されるオブジェクトの位置
	mark.x = x;
	mark.y = y;
	mark.z = z;
		
	for(var i=0; i<MAX_COUNTRY; i++) {
		if ( mark.position.x != flag[i].x && mark.position.y != flag[i].y && mark.position.z != flag[i].z ) {
			flagmarker = (flagmarker + 1) % MAX_COUNTRY;
			flag[i].x = x;
			flag[i].y = y;
			flag[i].z = z;
			console.log('flag_notexsist');
			mark.visible = true;
			break;
		}
		else {
			console.log('flag_exsist');
			mark.visible = true;
		}
	}
	
	mark.x.NeedUpdate = true;
	mark.y.NeedUpdate = true;
	mark.z.NeedUpdate = true;
	mark.visible = true;
	//mesh.rotation.x = MATH.PI/4;
	//scene.add(mesh);

}
*/


// --------------------------------------------- socket処理
function startNetwork() {
	// 接続開始
	socket = io.connect();
	
	// 接続時
	socket.on('connect', function() {
		console.log('start socket.io');
		//socket.emit('capture');
	});
	
	// 受信時イベント
	socket.on('packet', function(data) {
		var packet = JSON.parse(data);
			
		if(data[0] != '{') {
			console.log('aaaaaaaaaaaa');
		}
		else {
			if(packet.country != '??') {
				setFlow(packet);
				//console.log('country:OK')
			} else {
				setFlow({latlng:[-90, 0], white: 0, direction: packet.direction});
				//console.log('country:not OK')
			}
			//console.log(packet.country);
			insertInfoTable(packet);
			//console.log('aaaa');
			//sleep(0.1);
		}
	});
	
	socket.on('disconnect', function() {
		console.log('stopped socket.io');
	});
}

// パケット情報を表に追加する
function insertInfoTable(packet) {
	// 行の設定
	var $row = $("<tr></tr>")
		.append($("<td class='datetime'></td>").text(packet.datetime))
		.append($("<td class='country'></td>></td>").text(packet.country))
		.append($("<td class='s_addr'></td>></td>").text(packet.address[0]))
		.append($("<td class='d_addr'></td>></td>").text(packet.address[1]))
		.append($("<td class='protocol'></td>></td>").text(packet.protocol))
		.append($("<td class='length'></td>></td>").text(packet.length));
		//.append($("<td class='s_port'></td>></td>").text(packet.port[0]))
		//.append($("<td class='d_port'></td>></td>").text(packet.port[1]));
			
	// テーブル取得
	var $table = $('#infoTable tbody');
	// テーブルに行を追加
	$table.append($row);
	
	// 最下部までスクロール
	$table.scrollTop($table[0].scrollHeight);
}


})();

