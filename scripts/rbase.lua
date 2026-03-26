--
-- Copyright 2025 Milos Tosic. All rights reserved.
-- License: http://www.opensource.org/licenses/BSD-2-Clause
--

function projectExtraConfig_rbase() 
	includedirs	{ path.join(projectGetPath("rbase"), "../") }
end

function projectExtraConfigExecutable_rbase() 
	includedirs	{ path.join(projectGetPath("rbase"), "../") }
end

function projectAdd_rbase() 
	addProject_lib("rbase")
end
