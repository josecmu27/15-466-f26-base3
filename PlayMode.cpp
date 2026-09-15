#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint hexapod_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > music_match_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("music_match_meshes.pnct"));
	hexapod_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > music_match_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("music_match.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = music_match_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = hexapod_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< Sound::Sample > dusty_floor_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("dusty-floor.opus"));
});


Load< Sound::Sample > honk_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("honk.wav"));
});

/*---------------------------LOAD MUSIC NOTES-------------------------*/
Load< Sound::Sample > purple_music_note_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("purple-music-note.wav"));
});

Load< Sound::Sample > red_music_note_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("red-music-note.wav"));
});

Load< Sound::Sample > yellow_music_note_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("yellow-music-note.wav"));
});

Load< Sound::Sample > blue_music_note_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("blue-music-note.wav"));
});

Load< Sound::Sample > green_music_note_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("green-music-note.wav"));
});


/*---------------------------LOAD SOUND EFFECTS-------------------------*/

Load< Sound::Sample > correct_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("correct.wav"));
});


Load< Sound::Sample > incorrect_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("incorrect.wav"));
});

Load< Sound::Sample > gamewin_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("game-win.wav"));
});

Load< Sound::Sample > gamelose_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("game-lose.wav"));
});


PlayMode::PlayMode() : scene(*music_match_scene) {
	//get pointers to leg for convenience:
	// for (auto &transform : scene.transforms) {
	// 	if (transform.name == "Hip.FL") hip = &transform;
	// 	else if (transform.name == "UpperLeg.FL") upper_leg = &transform;
	// 	else if (transform.name == "LowerLeg.FL") lower_leg = &transform;
	// }

	// if (hip == nullptr) throw std::runtime_error("Hip not found.");
	// if (upper_leg == nullptr) throw std::runtime_error("Upper leg not found.");
	// if (lower_leg == nullptr) throw std::runtime_error("Lower leg not found.");

	// hip_base_rotation = hip->rotation;
	// upper_leg_base_rotation = upper_leg->rotation;
	// lower_leg_base_rotation = lower_leg->rotation;
	
	musicnote_samples = {
							*purple_music_note_sample,
							*red_music_note_sample,
							*yellow_music_note_sample,
							*blue_music_note_sample,
							*green_music_note_sample
						  };

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
	camera->transform->position += glm::vec3(5.0f, 0.0f, 0.0f);

	initial_camera_position = camera->transform->position;

	correct_notes = music_sheets.at(0);

	current_game_state = GameState::PreRound;
	current_game_message = "Memorize how each Key Sounds!";

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_EVENT_KEY_DOWN) {
		if (evt.key.key == SDLK_ESCAPE) {
			SDL_SetWindowRelativeMouseMode(Mode::window, false);
			return true;
		} else if (evt.key.key == SDLK_A) { // Purple
			if (current_game_state == GameState::Memorize) return false;
			if (purple_oneshot) purple_oneshot->stop();
			purple_oneshot = Sound::play_3D(*purple_music_note_sample, 0.3f, initial_camera_position);

			RegisterMusicNote(MusicNote::Purple);

			return true;
		} else if (evt.key.key == SDLK_S) { // Red
			if (current_game_state == GameState::Memorize) return false;

			if (red_oneshot) red_oneshot->stop();
			red_oneshot = Sound::play_3D(*red_music_note_sample, 0.3f, initial_camera_position);

			RegisterMusicNote(MusicNote::Red);

			return true;
		} else if (evt.key.key == SDLK_SPACE){ // Yellow
			if (current_game_state == GameState::Memorize) return false;

			if (yellow_oneshot) yellow_oneshot->stop();
			yellow_oneshot = Sound::play_3D(*yellow_music_note_sample, 0.3f, initial_camera_position);

			RegisterMusicNote(MusicNote::Yellow);

			return true;
		} else if (evt.key.key == SDLK_K){ // Blue
			if (current_game_state == GameState::Memorize) return false;

			if (blue_oneshot) blue_oneshot->stop();
			blue_oneshot = Sound::play_3D(*blue_music_note_sample, 0.3f, initial_camera_position);

			RegisterMusicNote(MusicNote::Blue);

			return true;
		} else if (evt.key.key == SDLK_L){ // Green
			if (current_game_state == GameState::Memorize) return false;

			if (green_oneshot) green_oneshot->stop();
			green_oneshot = Sound::play_3D(*green_music_note_sample, 0.3f, initial_camera_position);

			RegisterMusicNote(MusicNote::Green);
			
			return true;
		} else if (evt.key.key == SDLK_P){ // Start Game
			
			current_game_state = GameState::Memorize;
			return true;
		} 
	} else if (evt.type == SDL_EVENT_KEY_UP) {
		if (evt.key.key == SDLK_A) {
			// left.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_D) {
			// right.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_W) {
			// up.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_S) {
			// down.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (SDL_GetWindowRelativeMouseMode(Mode::window) == false) {
			SDL_SetWindowRelativeMouseMode(Mode::window, true);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	
	{ // play correct notes in sequence
		if (current_game_state == GameState::Memorize)
		{
			current_game_message = "Listen Carefully!";

			if (current_oneshot && !current_oneshot->stopped) return; 

			if (current_note_idx == 0)
			{
				current_oneshot = Sound::play_3D(musicnote_samples[correct_notes[0]], 0.3f, initial_camera_position);
				current_note_idx++;
				return;
			}
			
			if (current_oneshot->stopped && current_note_idx < correct_notes.size())
			{
				current_oneshot = Sound::play_3D(musicnote_samples[correct_notes[current_note_idx]], 0.3f, initial_camera_position);
				current_note_idx++;
				return;
			} 
			
			if (current_note_idx == correct_notes.size())
			{
				current_note_idx = 0;
				current_game_state = GameState::Play;
				current_game_message = "Your Turn!";
			}

		}
	}

	{ //update listener to camera position:
		glm::mat4x3 frame = camera->transform->make_parent_from_local();
		glm::vec3 frame_right = frame[0];
		glm::vec3 frame_at = frame[3];
		Sound::listener.set_position_right(frame_at, frame_right, 1.0f / 60.0f);
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Press 'P' to Play, escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));

		// Health Points
		lines.draw_text("Hit Points: ",
			glm::vec3(-aspect + 0.1f * H + ofs, 0.85f, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));

		lines.draw_text(std::to_string(hit_points),
			glm::vec3(-aspect + 0.1f * H + ofs + 0.35, 0.85f, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));

		
		// Game State Message
		lines.draw_text(current_game_message,
			glm::vec3(-0.50f, 0.3f, 0.0f),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
	}
	GL_ERRORS();
}

void PlayMode::RegisterMusicNote(MusicNote music_note)
{
	if (current_game_state != GameState::Play) return;

	played_notes.emplace_back(music_note);
	int current_idx = played_notes.size() - 1;


	if (played_notes.at(current_idx) != correct_notes.at(current_idx))
	{
		hit_points--;

		current_oneshot = Sound::play_3D(*incorrect_sample, 0.3f, initial_camera_position);

		// replay correct notes
		current_game_state = GameState::Memorize;

		played_notes.clear();
		return;
	}

	if (current_idx == correct_notes.size() - 1)
	{
		// Game Win
		if (music_sheet_index == music_sheets.size() - 1)
		{
			current_game_message = "You Win!";
			current_game_state = GameState::PostRound;

			std::shared_ptr< Sound::PlayingSample > win_oneshot = Sound::play_3D(*gamewin_sample, 0.3f, initial_camera_position);
			return;
		}

		current_oneshot = Sound::play_3D(*correct_sample, 0.3f, initial_camera_position);

		// Go to next music sheet
		current_game_message = "Hooray";
		music_sheet_index++;
		correct_notes = music_sheets.at(music_sheet_index);
		
		current_game_state = GameState::Memorize;
		played_notes.clear();
	}
	else
	{
		current_game_message = "Nice!";
	}

}

