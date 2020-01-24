#pragma once

/* Функция возвращает id потока, в котором вызвана */
#define current_thread_id() std::this_thread::get_id()

/* ? */
//#define try_to(x)   do { auto ret = x;\
//                        if (utils::exit_code(ret)) {\
//                            log_error("Function " << (#x)\
//                            << " failed with code: " << ret\
//                            << " - " << utils::code_to_string(ret));\
//                            return ret;\
//                        }\
//                    } while (false)

#define try_to(x)   do { auto ret = x;\
                        if (utils::exit_code(ret)) {\
                            if (utils::error_code(ret)) {\
                                log_error("Function " << (#x)\
                                << " failed with code: " << ret\
                                << " - " << utils::to_string(ret));\
                            }\
                            return ret;\
                        }\
                    } while (false)

/* ? */ //TODO throw fpp exception
#define try_throw(x)    do { auto ret = x;\
                            if (utils::exit_code(ret)) {\
                                log_error("Function " << (#x)\
                                << " failed with code: " << ret\
                                << " - " << utils::to_string(ret));\
                            }\
                        } while (false)

/* ? */
#define try_throw_static(x) do { auto ret = x;\
                                if (utils::exit_code(ret)) {\
                                    static_log_error("TODO", "Function " << (#x)\
                                    << " failed with code: " << ret\
                                    << " - " << utils::to_string(ret));\
                                }\
                            } while (false)

/* ? */ // В случае ошибки не возвращать ее код, а выбрасывать исключение (ex. в деструкторах); добавить параметр - текст ошибки
//#define try_and_throw(x,msg) { auto ret = x;\
//                    if (utils::exit_code(ret)) {\
//                        log_error("Function " << (#x)\
//                        << " failed with code: " << ret\
//                        << " - " << utils::code_to_string(ret));\
//                        return ret;\
//                    }\
//                  } SEMICOLON_REQUIREMENT

/* ? */
//TODO
// Возрат кода и отправка сообщения в лог, сответствующий коду

/* ? */
#define return_if(cond,ret_value)   do {\
                                        if (cond) {\
                                            return ret_value;\
                                        }\
                                    } while(false)

/* ? */
#define return_if_not(cond,ret_value)   do {\
                                            if (!(cond)) {\
                                                return ret_value;\
                                            }\
                                        } while (false)

/* ? */
#define return_info_if(cond,msg,ret_value)  do {\
                                                if (cond) {\
                                                    log_info(msg);\
                                                    return ret_value;\
                                                }\
                                            } while(false)

#define return_warning_if(cond,msg,ret_value)  do {\
                                                if (cond) {\
                                                    log_warning(msg);\
                                                    return ret_value;\
                                                }\
                                            } while(false)

//TODO объединить макросы в один, определять тип лога исходя из кода возврата 13.01
#define return_warning_if_not(cond,msg,ret_value)  do {\
                                                if_not(cond) {\
                                                    log_warning(msg);\
                                                    return ret_value;\
                                                }\
                                            } while(false)

#define return_error_if(cond,msg,ret_value)  do {\
                                                if (cond) {\
                                                    log_error(msg);\
                                                    return ret_value;\
                                                }\
                                            } while(false)

#define return_error_if_not(cond,msg,ret_value) do {\
                                                    if_not(cond) {\
                                                        log_error(msg);\
                                                        return ret_value;\
                                                    }\
                                                } while(false)

/* ? */
#define trow_if(cond,error_message) //TODO

/* ? */
#define if_not(x)       if(!(x))
#define while_not(x)    while(!(x))

/* Значения для задержек в коде в мс */
#define SHORT_DELAY_MS          10
#define MEDIUM_DELAY_MS         100  //TODO избавиться от ожидания в коде, где используются эти макросы 17.01
#define LONG_DELAY_MS           1000
