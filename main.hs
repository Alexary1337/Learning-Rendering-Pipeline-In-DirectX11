type Operator = Double -> Double -> Double
type Entry = (String, Operator)
type Register = [Entry]

operatorRegister :: Register
operatorRegister = [
                ("+", (+)),
                ("*", (*))
            ]
            
main = print $ calculate "2 + 2 * 2"
            
calculate :: String -> Double
calculate = calc operatorRegister . words
            
calc :: Register -> [String] -> Double
calc _ [number] = read number
calc ((operator, function):xs) unparsed =
    case span (/=operator) unparsed of
        (_, []) -> calc xs unparsed
        (beforeOperator, afterOperator) -> 
            function
                (calc operatorRegister beforeOperator)
                (calc operatorRegister $ drop 1 afterOperator)