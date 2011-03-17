@normal_symbol = external global i32
@weak_odr_symbol = weak_odr constant i32 42
;@weak_odr_symbol = external constant i32
@linkonce_odr_symbol = linkonce_odr constant i32 13

declare void @test_function(i32, i32, i32, i32*, i32*, i32*)

define i32 @main() nounwind {
entry:
	%0 = load i32* @normal_symbol
	%1 = load i32* @weak_odr_symbol
	%2 = load i32* @linkonce_odr_symbol
	call void @test_function(i32 %0, i32 %1, i32 %2,
     	  		         i32* @normal_symbol,
				 i32* @weak_odr_symbol,
				 i32* @linkonce_odr_symbol) nounwind
	ret i32 0
}
