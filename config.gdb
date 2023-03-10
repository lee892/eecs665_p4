set breakpoint pending on
set confirm off
file ./jeffc
break jeff::Err::report
commands
	where
end
break jeff::InternalError::InternalError
commands
	where
end

define p4
  set args p4_tests/$arg0.jeff -n
  run
end
