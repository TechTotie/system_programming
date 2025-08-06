/*
 * @brief: return codes
 * each program has a return code, like the main returns an int
 * the return codes of anything that is run can be access by '$?' variable.
 * normal return is zero value
 * non zero value means error
 * example:
 * %ls --> lists file
 * %echo $? --> output is 0
 * %ls bogusfile --> error, no such file or directory
 * %echo $? --> output is 1 or 2 depends on system/bash, indicating error
*/
