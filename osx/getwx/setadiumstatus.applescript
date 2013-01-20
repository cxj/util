on run argv
	if appIsRunning("Adium") then
		tell application "Adium"
			set status message of every account to argv
		end tell
	end if
	if appIsRunning("Skype") then
		tell application "AppleScript Editor"
			set cmd to "SET PROFILE MOOD_TEXT " & argv
			«event sendskyp» given «class cmnd»:cmd, «class scrp»:"setadiumstatus"
		end tell
	end if
end run

on appIsRunning(appName)
	tell application "System Events" to (name of processes) contains appName
end appIsRunning
