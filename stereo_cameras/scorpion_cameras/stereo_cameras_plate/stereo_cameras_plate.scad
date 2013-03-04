thickness=6;
detail=60;
// camera mounting hole radius
chrad = 3.3;

module camera_mount() {
	circle(r=chrad,
		$fn=detail, center=true);
	translate([-12.5, -12.5, 0])
	circle(r=chrad,
		$fn=detail, center=true);
	translate([-12.5, 12.5, 0])
	circle(r=chrad,
		$fn=detail, center=true);
	translate([12.5, 12.5, 0])
	circle(r=chrad,
		$fn=detail, center=true);
	translate([12.5, -12.5, 0])
	circle(r=chrad,
		$fn=detail, center=true);
}


difference() {
	// base
	square(size=[330.2, 60], center=true);

	// cnc mounting holes
	translate([-152.4, -12.7])
	circle(r=3.26,
		$fn=detail, center=true);

	translate([-152.4, 12.7])
	circle(r=3.26,
		$fn=detail, center=true);

	// cnc mounting holes
	translate([152.4, -12.7])
	circle(r=3.26,
		$fn=detail, center=true);

	translate([152.4, 12.7])
	circle(r=3.26,
		$fn=detail, center=true);

	translate([-100, 0])
	camera_mount();
	
	translate([100, 0])
	camera_mount();
}