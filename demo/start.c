/*************************************************************************************
 * 文件:    start.c
 *
 * 说明:    应用程序启始例程
 *
 * 作者:    Jun
 *
 * 时间:    2011-2-19
*************************************************************************************/
#define  APP_START_C
#include <FamesOS.h>


void __task demo_task(void * data)
{
    int i;

    i = (int)data;

    for(;;){
        lock_kernel();
        printf("Task Data: %d\n", i);
        unlock_kernel();
        TaskDelay(100*i);
    }
}

enum {
    __id_name,
    __id_sex,
    __id_old,
    __id_sss,
};

DECLARE_INPUT_DIALOG_START(abcde)
INPUT_DIALOG_ITEM("姓名: ", __id_name, 8, 10,   40, 120, 32, 0, 0, "XXXXXXXXXXXXX")
INPUT_DIALOG_ITEM("性别: ", __id_sex,  8, 10,   80, 120, 32, 0, 0, "XXXXXXXXXXXXX")
INPUT_DIALOG_ITEM("年龄: ", __id_old,  8, 10,  120, 120, 32, 0, 0, "XXXXXXXXXXXXX")
INPUT_DIALOG_ITEM("姓名: ", __id_name, 8, 150,  40, 120, 32, 0, 0, "XXXXXXXXXXXXX")
INPUT_DIALOG_ITEM("性别: ", __id_sex,  8, 150,  80, 120, 32, 0, 0, "XXXXXXXXXXXXX")
INPUT_DIALOG_ITEM("年龄: ", __id_old,  8, 150, 120, 120, 32, 0, 0, "XXXXXXXXXXXXX")
INPUT_DIALOG_ITEM("姓名: ", __id_name, 8, 280,  40, 120, 32, 0, 0, "XXXXXXXXXXXXX")
INPUT_DIALOG_ITEM("性别: ", __id_sex,  8, 280,  80, 120, 32, 0, 0, "XXXXXXXXXXXXX")
INPUT_DIALOG_ITEM("年龄: ", __id_old,  8, 280, 120, 120, 32, 0, 0, "XXXXXXXXXXXXX")
INPUT_DIALOG_SET(abcde, "输入对话框演示", NULL, 100, 100, 500, 250, 0, 0, 0, FORM_STYLE_TITLE|FORM_STYLE_XP_BORDER);
DECLARE_INPUT_DIALOG_ENDED(abcde)

gui_widget * form;
gui_widget * edit;
gui_widget * test;
gui_widget * view;

gui_widget * progress1;

gui_widget * dialog;

BMPINFO icon;
    
INT16U style = 0;
COLOR  color = 0, bkcolor = 0;

#define __style DRAW_OPT_ALIGN_CENTER|DRAW_OPT_FIL_BG

view_fields_t fields[] = {
    { "姓名", __id_name, 8,  9, 0, __style, "学生的姓名，4个汉字",},
    { "性别", __id_sex,  1,  5, 0, __style, "M/F",},
    { "年龄", __id_old,  3,  6, 0, __style, "",},
    { "备注", __id_sss, 28, 30, 0, __style, "学生备注",},
    { NULL, },
};

struct student
{
    char name[10];
    char sex;
    int  old;
    char comment[128];
    struct student * next;
};

struct student * all_students = NULL;

int get_max_index(void)
{
    int i = -1;
    struct student * t;

    t = all_students;

    lock_kernel();
    while(t){
        i++;
        t = t->next;
    }
    unlock_kernel();

    return i;
}

BOOL get_item(int index, int field_id, char *buf, int buf_len, INT16U option)
{
    struct student * t;
    int i;

    FamesAssert(buf);

    if(index > get_max_index())
        return fail;

    t = all_students;
    for(i=0; i<index; i++){
        if(t == NULL)
            return fail;
        t = t->next;
    }
    if(t == NULL)
            return fail;

    switch(field_id){
        case __id_name:
            strcpy(buf, t->name);
            break;
        case __id_sex:
            sprintf(buf, "%c", t->sex);
            break;
        case __id_old:
            sprintf(buf, "%d", t->old);
            break;
        case __id_sss:
            sprintf(buf, "%s", t->comment);
            break;
        default:
            break;
    }

    return ok;
}

BOOL set_item(int index, int field_id, char *buf, int buf_len, KEYCODE key, INT16U option)
{
    struct student * t;
    int i;

    FamesAssert(buf);

    if(index > get_max_index())
        return fail;

    t = all_students;
    for(i=0; i<index; i++){
        if(t == NULL)
            return fail;
        t = t->next;
    }
    
    if(t == NULL)
            return fail;

    switch(field_id){
        case __id_name:
            strcpy(t->name, buf);
            break;
        case __id_sex:
            t->sex = buf[0];
            break;
        case __id_old:
            t->old = atoi(buf);
            break;
        case __id_sss:
            sprintf(t->comment, buf);
            break;
        default:
            break;
    }

    return ok;
}

BOOL is_writable(int index, int field_id, INT16U option)
{
    return ok;
}

void show_status(int index, int x, int y, int width, int height, 
                                    int color, int bkcolor, int font,
                                    INT08S *field_comment,
                                    INT08S *old, INT16U option)
{
    char s[64];

    sprintf(s, "编号: %d, 栏位: %s", index+1, field_comment);
    
    draw_font_for_widget(x, y, width, height, s, old, 189, bkcolor, font, DRAW_OPT_FIL_BG);
}

void notifier(int index, int prev_index, INT16U option)
{
    int i;

    if(get_max_index() < 0)
        return;

    i = index+1;

    i *= 100;
    i /= (get_max_index()+1);

    gui_progress_set_value(progress1, i);

    gui_edit_set_text(edit, "又修改了一次");
    
}

void init_view(gui_widget * view)
{
    gui_view_init_private(view, 
                          fields, 
                          get_max_index, 
                          get_item, 
                          set_item, 
                          is_writable, 
                          NULL, 
                          "统计",
                          show_status,
                          notifier,
                          90,
                          66,
                          30,
                          1);
}


void demo_init_gui(void)
{
    InitBMPINFO(&icon);
    LoadBmp(&icon, "icon.bmp");
    
    form = gui_create_widget(GUI_WIDGET_FORM, 160, 150, 622, 420, 0, 0, 0, FORM_STYLE_XP_BORDER|FORM_STYLE_TITLE);
    if(!form)
        goto err;
    gui_form_init_private(form, 64);
    gui_form_set_caption(form, "控件演示");
    gui_form_set_icon(form, &icon);

    edit = gui_create_widget(GUI_WIDGET_EDIT, 80, 270, 460, 48, color, bkcolor, 0, style);
    if(!edit)
        goto err;
    gui_edit_init_private(edit, 128);
    gui_edit_set_text(edit, "这是一个文本框");

    test = gui_create_widget(GUI_WIDGET_EDIT, 80, 330, 460, 32, 0, 0, 0, 0);
    if(!test)
        goto err;
    gui_edit_init_private(test, 128);
    gui_edit_set_text(test, "");

    view = gui_create_widget(GUI_WIDGET_VIEW, 80, 48, 460, 200, 0, 0, 0, VIEW_STYLE_NONE_FIRST|VIEW_STYLE_FIELDS_TITLE|VIEW_STYLE_STATISTIC_BAR);
    if(!view)
        goto err;
    init_view(view);

    progress1 = gui_create_widget(GUI_WIDGET_PROGRESS, 80, 251, 460, 14, 0, 0, 0, PROGRESS_STYLE_BORDER|PROGRESS_STYLE_SMOOTH);
    if(!progress1)
        goto err;
    gui_progress_init_private(progress1, 100);

    dialog = input_dialog_initialize(&abcde);
    if(!dialog)
        goto err;

    gui_widget_link(NULL, form);
    gui_widget_link(form, edit);
    gui_widget_link(form, test);
    gui_widget_link(form, view);

    gui_widget_link(NULL, dialog);

    gui_widget_link(form, progress1);

    gui_set_root_widget(form);

    StartGUI();
    
err:
    return;
}


/*------------------------------------------------------------------------------------
 * 函数:    start()
 *
 * 描述:    应用程序启始例程
**----------------------------------------------------------------------------------*/
void __task start(void * data)
{
    data = data;

    /*
    TaskCreate(demo_task, (void *)1, "demo1", NULL, 512, 1, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)2, "demo2", NULL, 512, 2, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)3, "demo3", NULL, 512, 3, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)4, "demo4", NULL, 512, 4, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)5, "demo5", NULL, 512, 5, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)6, "demo6", NULL, 512, 6, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)7, "demo7", NULL, 512, 7, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)8, "demo8", NULL, 512, 8, TASK_CREATE_OPT_NONE);
    */

    demo_init_gui();

    OpenConsole();

    for(;;){
        KEYCODE key;
        char buf[64] = {0, };

        key = gui_edit_input(test, buf, 32, 0);
        switch(key){
            case UP:
                gui_view_move_up(view);
                break;
            case DOWN:
                gui_view_move_down(view);
                break;
            case PGUP:
                gui_view_page_up(view);
                break;
            case PGDN:
                gui_view_page_down(view);
                break;
            case ESC:
                ExitApplication();
                break;
        }
        switch(buf[0]){
            default:
                break;
            case 'c':
            case 'C':
                color = atoi(&buf[1]);
                gui_set_widget_color(edit, color);
                break;
            case 'b':
            case 'B':
                bkcolor = atoi(&buf[1]);
                gui_set_widget_bkcolor(edit, bkcolor);
                break;
            case 's':
            case 'S':
                style = STRtoINT16(&buf[1], 0);
                gui_set_widget_style(edit, style);
                break;
            case 'p':
            case 'P':
                gui_progress_set_value(progress1, atoi(&buf[1]));
                break;
            case 'a':
            case 'A':
                {
                     struct student * t;

                     t = mem_alloc(sizeof(*t));
                     if(!t)
                        break;
                     sprintf(t->name, "Name");
                     t->sex = '-';
                     t->old = 0;
                     sprintf(t->comment, "");
                     lock_kernel();
                     t->next = all_students;
                     all_students = t;
                     unlock_kernel();
                }
                break;
            case 'd':
            case 'D':
                gui_set_root_widget(dialog);
                waitkey(0L);
                gui_set_root_widget(form);
                break;
            case 'e':
            case 'E':
                while(1){
                    key = gui_view_editing(view, 0);
                    if(key == ESC)
                        break;
                    switch(key){
                        case UP:
                            gui_view_move_up(view);
                            break;
                        case DOWN:
                            gui_view_move_down(view);
                            break;
                        default:
                            break;
                    }
                }
                break;
        }
    }
}

/*------------------------------------------------------------------------------------
 * 函数: TaskSwitchHook()
 *
 * 说明: 任务切换钩子
 *
 * 特别: 此函数运行在中断服务程序之中,应特别注意其执行效率
**----------------------------------------------------------------------------------*/
void apical TaskSwitchHook(void)
{
}

void quit(void)
{
    ExitApplication();
}

long get_free_mem(void)
{
    long mem;

    lock_kernel();
    mem = (long)coreleft();
    unlock_kernel();

    return mem;
}

/*------------------------------------------------------------------------------------
 *  取系统的时间日期
**----------------------------------------------------------------------------------*/
void GetDateTime (INT08S *s)
{
    struct time now;
    struct date today;

    lock_kernel();
    gettime(&now);
    getdate(&today);
    unlock_kernel();
    sprintf(s, "%02d-%02d-%02d  %02d:%02d:%02d",
            today.da_mon,
            today.da_day,
            today.da_year,
            now.ti_hour,
            now.ti_min,
            now.ti_sec);
}


/*====================================================================================
 * 
 * 本文件结束: start.c
 * 
**==================================================================================*/

