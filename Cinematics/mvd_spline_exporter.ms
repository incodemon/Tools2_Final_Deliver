filein "mvd_json_exporter.ms"

global MaxToMVD = rotateXMatrix -90

--Exporter Scripts

fn exportSpline obj =
(
	total_knots = #()
	comp_spline = MVDJson()

	for s = 1 to (numSplines obj) do(
		for k = 1 to (numKnots obj s) do(
			local knot = (getKnotPoint obj s k) * MaxToMVD
			append total_knots knot
		)
	)
	print total_knots
)


exportSpline $