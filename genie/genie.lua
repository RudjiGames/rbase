--
-- Copyright 2025 Milos Tosic. All rights reserved.
-- License: http://www.opensource.org/licenses/BSD-2-Clause
--

newoption { trigger = "zidar-path", description = "Path to zidar" }

if os.isfile("../../zidar/zidar.lua") and not _OPTIONS["zidar-path"]then
	dofile "../../zidar/zidar.lua"
else
	dofile(_OPTIONS["zidar-path"] .. "/zidar.lua")
end

dofile "rbase.lua"

solution "rbase"
	setPlatforms()

	addLibProjects("rbase")
