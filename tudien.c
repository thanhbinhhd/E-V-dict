#include <gtk/gtk.h>
#include "btree.h"
#include <string.h>
#include "soundEx.h"

BTA *data;
GtkWidget *textView,*mainwindow;
GtkTextBuffer *buffer;

GtkWidget* gtk_button_with_icon_and_label(char* label,char* fileimage){
  GtkWidget *button,*image;
  image=gtk_image_new();
  gtk_image_set_from_file(GTK_IMAGE(image),fileimage);
  button=gtk_button_new_with_label(label);
  gtk_button_set_image((GtkButton *)button,image);
  gtk_button_set_image_position((GtkButton *)button,GTK_POS_LEFT);
  return button;
}

//Tao icon ghep vao button
GdkPixbuf *create_pixbuf(const gchar *filename){
  GdkPixbuf *pixbuf;
  GError *error=NULL;
  pixbuf=gdk_pixbuf_new_from_file(filename,&error);
  if (!pixbuf){
    fprintf(stderr,"%s\n",error->message);
    g_error_free(error);
  }
  return pixbuf;
}
void insert_word(GtkWidget *widget,gpointer entry){
  GtkWidget *dialog,*window=mainwindow;
  char word[50],meaning[10000];
  BTint value;
  GtkTextIter start,end;
  strcpy(word,(char*)gtk_entry_get_text(GTK_ENTRY(entry)));
  gtk_text_buffer_get_start_iter(buffer,&start);
  gtk_text_buffer_get_end_iter(buffer,&end);
  strcpy(meaning,(char*)gtk_text_buffer_get_text(buffer,&start,&end,0));
  if (strlen(word)==0){
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Bạn chưa nhập từ!");
    gtk_window_set_title(GTK_WINDOW(dialog),"Lỗi!");
    gtk_dialog_run(GTK_DIALOG(dialog));
  }
  else 
  if (strlen(meaning)==0) {
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Bạn chưa nhập nghĩa!");
    gtk_window_set_title(GTK_WINDOW(dialog),"Lỗi!");
    gtk_dialog_run(GTK_DIALOG(dialog));
  } 
  else
  if (bfndky(data,word,&value)!=QNOKEY){
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Từ đã có trong cơ sở dữ liệu!Bạn muốn cập nhập nghĩa của từ này?");
    gtk_window_set_title(GTK_WINDOW(dialog),"Trùng từ");
    if (gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_YES)
        btupd(data,word,meaning,strlen(meaning)+1);
  }
  else{
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Bạn muốn thêm từ này vào cơ sở dữ liệu?");
    gtk_window_set_title(GTK_WINDOW(dialog),"Xác nhận");
    if (gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_YES) {
      btins(data,word,meaning,strlen(meaning)+1);
    }
  }
  gtk_widget_destroy(dialog);
}

void delete_word(GtkWidget *widget, gpointer entry){
  GtkWidget *dialog,*window=mainwindow;
  char word[50];
  BTint value;
  strcpy(word,(char*)gtk_entry_get_text(GTK_ENTRY(entry)));
  if (strlen(word) == 0){
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Bạn chưa nhập từ!");
    gtk_window_set_title(GTK_WINDOW(dialog),"Lỗi!");
    gtk_dialog_run(GTK_DIALOG(dialog));
  }
  else if (bfndky(data,word,&value)==0){
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Bạn muốn xóa từ này khỏi cơ sở dữ liệu?");
    gtk_window_set_title(GTK_WINDOW(dialog),"Xác nhận");
    if (gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_YES) {
      btdel(data, word);
      gtk_text_buffer_set_text(buffer,"",-1);
      gtk_entry_set_text(GTK_ENTRY(entry),"");
    }
  }
  else{
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Từ không có trong cơ sở dữ liệu!");
    gtk_window_set_title(GTK_WINDOW(dialog),"Lỗi!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    }
  gtk_widget_destroy(dialog);
  //return FALSE;
}

void *show_meaning(char *meaning){
  FILE *stream;
  GtkTextIter iter;
  char line[2000];
  static int created=0;
  // Ham chuyen 1 string tuong duong thanh 1 file
  stream=fmemopen(meaning,strlen(meaning)+1,"r");
  gtk_text_buffer_set_text(buffer,"",-1);
  gtk_text_buffer_get_start_iter(buffer,&iter);
  //Tag kieu chu
  if (!created){
    gtk_text_buffer_create_tag(buffer,"lmarg15","left_margin",15,NULL);
    gtk_text_buffer_create_tag(buffer,"lmarg30","left_margin",30,NULL);
    gtk_text_buffer_create_tag(buffer,"lmarg35","left_margin",35,NULL);
    gtk_text_buffer_create_tag(buffer,"bold","weight",PANGO_WEIGHT_HEAVY,NULL);
    created=1;
  }
  //In nghia
  while (fgets(line,2000,stream)!=NULL){
    switch(line[0]){
    case '@':
      line[0]=' ';
      gtk_text_buffer_insert(buffer,&iter,"\n",-1);
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"bold",NULL);
      gtk_text_buffer_insert(buffer,&iter,"\n",-1);
      break;
    case '*':
      line[0]=' ';
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"lmarg15",NULL);
      break;
    case '!':
      line[0]=' ';
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"lmarg15",NULL);
      break;
    case '-':
      line[0]=' ';
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"lmarg30",NULL);
      break;
    case '=':
      line[0]=' ';
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"lmarg35",NULL);
      break;
    default:
      gtk_text_buffer_insert(buffer,&iter,line,-1);
      break;
    }
  }
}

void display(GtkWidget *entryword,gpointer database){
  GtkWidget *dialog,*window=mainwindow;
  char word[50],mean[10000];
  int value;
  strcpy(word,(char *)gtk_entry_get_text(GTK_ENTRY(entryword)));
  if (strlen(word) == 0) {
    gtk_text_buffer_set_text(buffer,"", -1);
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"The entry is empty. Please enter some word!\nBạn chưa nhập từ nào cả. Hãy nhập đi nhé.\n");
    gtk_window_set_title(GTK_WINDOW(dialog),"Nhập từ");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    //strcpy(mean,"The entry is empty. Please entry some word!\nBạn chưa nhập từ nào cả. Hãy nhập đi nhé.\n");
    }
    else{
  if (bfndky(data,word,(BTint *)&value)==0){
    btsel(data,word,mean,10000,&value);
    show_meaning(mean);
  }else{
    gtk_text_buffer_set_text(buffer,"", -1);
    char text_id[50],prev_word_id[50],next_word_id[50],prev_word[50],next_word[50];
    char suggest[1000]="Sorry! I'm not found this word in the database.\nXin lỗi! Không tìm thấy từ này trong dữ liệu\n";
/*      int i;
      strcpy(next_word, word) ;
      strcpy(prev_word, word) ;
      SoundEx(text_id,word,4,4);
      printf("text_id");
      int count = 5 ;
      for (i = 0 ; (i < 5000 ) && count ; i++){
        if (bprvky(data, prev_word, &value) == 0) {   // Tim thay key truoc va luu vao prevword voi value
          SoundEx(prev_word_id,prev_word,4,4);
          if (strcmp(prev_word_id,text_id)==0 && strcmp(prev_word,word)) {
            count--;
            strcat(suggest,prev_word);
            strcat(suggest,"\n");
          }
        }
        if (bnxtky(data, next_word, &value) == 0) {        // Tim next key va luu vao nextword voi value
          SoundEx(next_word_id,next_word,4,4);
          if (strcmp(next_word_id,text_id)==0 && strcmp(next_word,word)) {
            count--;
            strcat(suggest,next_word);
            strcat(suggest,"\n");
          }      
        }
      }
      */
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,suggest);
    gtk_window_set_title(GTK_WINDOW(dialog),"Xin lỗi");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    //strcpy(mean,"Sorry! I'm not found this word in the database.\nXin lỗi! Không tìm thấy từ này trong dữ liệu\n");
  }
  }
  
}

void search_word(GtkWidget *button,gpointer database){
  display(GTK_WIDGET(database),NULL);
}

void clearAll(GtkWidget *widget,gpointer database){
    gtk_text_buffer_set_text(buffer,"", -1);
    gtk_entry_set_text(GTK_ENTRY(database),"");
}

void show_info(GtkWidget *widget,gpointer database){
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_file("thongtin.ico",NULL);
  GtkWidget *dialog=gtk_about_dialog_new();
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog),"Từ điển Anh Việt - Việt Anh\nVietnamese <-> English dictionary");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog),"Version 1.0");
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog)," © Việt Nhật C K60\n714PV_Plus Team Code");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),"Chương trình được tham khảo code giao diện từ một số nguồn có sẵn trên mạng.\n");
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog),pixbuf);
  g_object_unref(pixbuf),pixbuf=NULL;
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

void show_guide(GtkWidget *widget,gpointer database){
  GtkWidget *dialog,*window;
  window=mainwindow;
  dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"1. Để tra từ, nhập từ sau đó nhấn Enter hoặc click vào nút Tra từ\n2. Để thêm từ, nhập từ và nghĩa sau đó click vào nút Thêm từ\n3. Để xóa từ, nhập từ và click vào nút Xóa từ\n4. Để thay đổi từ điển click vào từ điển bạn muốn tra\n5. Để đưa về màn hình ban đầu, click vào nút Khôi phục\n");
  gtk_window_set_title(GTK_WINDOW(dialog),"Hướng dẫn sử dụng");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

void AnhViet(){
  btcls(data);
  data=btopn("AnhViet.dat",0,0);
}

void VietAnh(){
  btcls(data);
  data=btopn("VietAnh.dat",0,0);
}

int isPrefix(char *s,char *word){
  int len1,len2,i;
  len1=strlen(s);
  len2=strlen(word);
  if (len1 > len2) return 0;
  for (i=0;i<len1;i++){
    if (s[i]!=word[i])
      return 0;
    }
  return 1;
}

void searchSuggest(GtkWidget *entryword,GdkEvent *event,gpointer listword){
  GdkEventKey *key=(GdkEventKey *)event;
  GtkListStore *liststore=(GtkListStore *)listword;
  GtkTreeIter iter;
  int count=0,len;
  BTint value;
  char text[50],near_word[50],next_word[50],prev_word[50];
  if (key->keyval==GDK_KEY_Tab){
    strcpy(text,gtk_entry_get_text(GTK_ENTRY(entryword)));
    if (bfndky(data,text,&value)==QNOKEY){
      // ham bnxtky tim key gan nhat voi key muon tim
     bnxtky(data,near_word,&value);
      if (isPrefix(text,near_word)){
	     gtk_entry_set_text(GTK_ENTRY(entryword),near_word);
	     gtk_editable_set_position(GTK_EDITABLE(entryword),strlen(near_word));
      }
    }
    display(entryword,NULL);
  }
  else
  {
    count=0;
    strcpy(text,gtk_entry_get_text(GTK_ENTRY(entryword)));
    if (key->keyval!=GDK_KEY_BackSpace){
        len=strlen(text);
      text[len]=key->keyval;
      text[len+1]='\0';
    }
    gtk_list_store_clear(liststore);
    if (bfndky(data,text,&value)==0) {
      gtk_list_store_append(liststore,&iter);
      gtk_list_store_set(liststore,&iter,0,text,-1);
    }
    bnxtky(data,near_word,&value);
    //if (isPrefix(text,near_word)!=0) {
    while ((isPrefix(text,near_word)!=0) && (count < 10)){
        gtk_list_store_append(liststore,&iter);
        gtk_list_store_set(liststore,&iter,0,near_word,-1);
        bnxtky(data,near_word,&value);
        count++;
    }
  }
}


int main(int argc,char *argv[]){
  GtkWidget *window,*mainvbox,*hbox,*vbox;
  GtkWidget *button,*entry,*label,*frame;
  GtkWidget *ins,*del,*gui,*info,*clear;
  // Khoi tao giao dien
  gtk_init(&argc,&argv);
  data=btopn("AnhViet.dat",0,FALSE);
  window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request(window,800,600);
  gtk_container_set_border_width(GTK_CONTAINER(window),10);
  gtk_window_set_title(GTK_WINDOW(window),"PVDict");
  gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
  gtk_window_set_icon(GTK_WINDOW(window),create_pixbuf("main.png"));
  g_signal_connect(window,"delete-event",G_CALLBACK(gtk_main_quit),NULL);
  mainwindow=window;
  // Ket thuc khoi tao
  //Tao box chinh cho tu dien
  mainvbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
  gtk_container_add(GTK_CONTAINER(window),mainvbox);

  // Tao box nhap tu va chon tu dien
  // Tao frame nhap tu
  frame=gtk_frame_new("Nhập từ");
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_IN);
  //Tao box
  hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

  GtkEntryCompletion *completion=gtk_entry_completion_new();
  gtk_entry_completion_set_text_column(completion,0);
  GtkListStore *liststore=gtk_list_store_new(5,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);
  gtk_entry_completion_set_model(completion,GTK_TREE_MODEL(liststore));

  entry=gtk_search_entry_new();
  gtk_entry_set_completion(GTK_ENTRY(entry),completion);
  g_signal_connect(entry,"activate",G_CALLBACK(display),NULL);
  g_signal_connect(entry,"key-press-event",G_CALLBACK(searchSuggest),liststore);
  gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,0);

  GtkSettings *default_settings=gtk_settings_get_default();
  g_object_set(default_settings,"gtk-button-images",TRUE,NULL);

  button=gtk_button_with_icon_and_label("Search","search.ico");
  g_signal_connect(button,"clicked",G_CALLBACK(search_word),(gpointer)entry);
  gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);

  //button=gtk_button_new_with_label("Từ điển Việt Anh");
  //g_signal_connect(button,"clicked",G_CALLBACK(VietAnh),(gpointer)entry);
  //gtk_box_pack_end(GTK_BOX(hbox),button,FALSE,FALSE,10);

  button=gtk_button_new_with_label("Từ điển Anh Việt");
  g_signal_connect(button,"clicked",G_CALLBACK(AnhViet),(gpointer)entry);
  gtk_box_pack_end(GTK_BOX(hbox),button,FALSE,FALSE,10);

  button=gtk_button_new_with_label("Từ điển Việt Anh");
  g_signal_connect(button,"clicked",G_CALLBACK(VietAnh),(gpointer)entry);
  gtk_box_pack_end(GTK_BOX(hbox),button,FALSE,FALSE,10);

  gtk_container_add(GTK_CONTAINER(frame),hbox);
  gtk_box_pack_start(GTK_BOX(mainvbox),frame,FALSE,FALSE,0);
  //

  // Tao box cho nghia va tac dung khac
  hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
  vbox=gtk_box_new(TRUE,1);

  ins=gtk_button_with_icon_and_label("Thêm từ","add.ico");
  g_signal_connect(ins,"clicked",G_CALLBACK(insert_word),(gpointer)entry);

  del=gtk_button_with_icon_and_label("Xóa từ","delete.ico");
  g_signal_connect(del,"clicked",G_CALLBACK(delete_word),(gpointer)entry);

  gui=gtk_button_with_icon_and_label("Hướng dẫn","huongdan.ico");
  g_signal_connect(gui,"clicked",G_CALLBACK(show_guide),(gpointer)entry);

  info=gtk_button_with_icon_and_label("Thông tin","thongtin.ico");
  g_signal_connect(info,"clicked",G_CALLBACK(show_info),(gpointer)entry);

  clear=gtk_button_with_icon_and_label("Clean","clear.jpg");
  g_signal_connect(clear,"clicked",G_CALLBACK(clearAll),(gpointer)entry);

  gtk_box_pack_start(GTK_BOX(vbox),ins,TRUE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(vbox),del,TRUE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(vbox),gui,TRUE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(vbox),info,TRUE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(vbox),clear,TRUE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(hbox),vbox,FALSE,FALSE,0);

  //Tao hop hien nghia
  GtkWidget* scrolledWindow=gtk_scrolled_window_new(NULL,NULL);
  gtk_container_set_border_width(GTK_CONTAINER (scrolledWindow),10);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  textView=gtk_text_view_new();
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView),GTK_WRAP_WORD);
  buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
  //gtk_text_buffer_set_text(buffer,"Xin chào!\nWelcome to PVDict",-1);
  gtk_container_add(GTK_CONTAINER(scrolledWindow),textView);
  gtk_box_pack_start(GTK_BOX(hbox),scrolledWindow,TRUE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(mainvbox),hbox,TRUE,TRUE,0);
  //
  gtk_widget_show_all(window);
  gtk_main();
  btcls(data);
  return 0;
}