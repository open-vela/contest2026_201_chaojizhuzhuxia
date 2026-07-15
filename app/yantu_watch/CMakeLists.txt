# yantu_watch - 研途计时・离线考研智能穿戴手表
if(CONFIG_LVX_USE_DEMO_CONTEST2026_201_YANTU_WATCH)
  nuttx_add_application(
    NAME yantu_watch
    SRCS
      main.c
      ui/ui_main.c
      ui/ui_countdown.c
      ui/ui_pomodoro.c
      ui/ui_vocabulary.c
      ui/ui_audio.c
      ui/ui_reminder.c
      ui/ui_study_plan.c
      ui/ui_settings.c
      ui/ui_components.c
      core/countdown_manager.c
      core/pomodoro_timer.c
      core/vocabulary_engine.c
      core/audio_player.c
      core/reminder_manager.c
      core/study_plan_manager.c
      core/study_record.c
      core/vibrator.c
      data/storage.c
      data/json_parser.c
      data/database.c
      ble/ble_sync.c
      ble/ble_protocol.c
    STACKSIZE 16384
    PRIORITY 100)
endif()