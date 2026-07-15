############################################################################
# Contest 2026 team 201 - yantu_watch Makefile
############################################################################

include $(APPDIR)/Make.defs

PROGNAME  = yantu_watch
PRIORITY  = 100
STACKSIZE = 16384
MODULE    = $(CONFIG_LVX_USE_DEMO_CONTEST2026_201_YANTU_WATCH)

MAINSRC = main.c

# UI layer
CSRCS += ui/ui_main.c ui/ui_countdown.c ui/ui_pomodoro.c
CSRCS += ui/ui_vocabulary.c ui/ui_audio.c ui/ui_reminder.c
CSRCS += ui/ui_study_plan.c ui/ui_settings.c ui/ui_components.c

# Core business logic
CSRCS += core/countdown_manager.c core/pomodoro_timer.c
CSRCS += core/vocabulary_engine.c core/audio_player.c
CSRCS += core/reminder_manager.c core/study_plan_manager.c
CSRCS += core/study_record.c core/vibrator.c

# Data layer
CSRCS += data/storage.c data/json_parser.c data/database.c

# BLE layer
CSRCS += ble/ble_sync.c ble/ble_protocol.c

include $(APPDIR)/Application.mk